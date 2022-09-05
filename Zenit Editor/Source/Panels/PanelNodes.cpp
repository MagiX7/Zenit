#include "PanelNodes.h"
#include "PanelInspector.h"

#include "Nodes/ColorNode.h"
#include "Nodes/ComputeShaderNode.h"
#include "Nodes/NoiseNode.h"
#include "Nodes/VoronoiNode.h"
#include "Nodes/NormalMapNode.h"
#include "Nodes/CircleNode.h"

#include "Nodes/Constants/Vec1Node.h"

#include "Nodes/Operators/BlendNode.h"
#include "Nodes/Operators/ClampNode.h"

#include "EditorLayer.h"

#define OUTPUT_NODE_ID 1
#define OUTPUT_ALBEDO_PIN_ID 2
#define OUTPUT_NORMALS_PIN_ID 3
#define OUTPUT_METALLIC_PIN_ID 4
#define OUTPUT_ROUGHNESS_PIN_ID 5

namespace Zenit {

	PanelNodes::PanelNodes(EditorLayer* edLayer) : editorLayer(edLayer)
	{
		config = ed::Config();
		config.SettingsFile = "Settings/NodeEditor.json";
		config.NavigateButtonIndex = 2;
		context = ed::CreateEditor(&config);
		ed::SetCurrentEditor(context);


		Node* node = new Node(OUTPUT_NODE_ID, "Output", NodeOutputType::NONE);

		Pin albedo = Pin(OUTPUT_ALBEDO_PIN_ID, "Albedo", PinType::Object, ed::PinKind::Input);
		albedo.node = node;
		node->inputs.emplace_back(albedo);

		Pin normals = Pin(OUTPUT_NORMALS_PIN_ID, "Normals", PinType::Object, ed::PinKind::Input);
		normals.node = node;
		node->inputs.emplace_back(normals);

		Pin metallic = Pin(OUTPUT_METALLIC_PIN_ID, "Metallic", PinType::Object, ed::PinKind::Input);
		metallic.node = node;
		node->inputs.emplace_back(metallic);

		Pin roughness = Pin(OUTPUT_ROUGHNESS_PIN_ID, "Roughness", PinType::Object, ed::PinKind::Input);
		roughness.node = node;
		node->inputs.emplace_back(roughness);

		nodes.emplace_back(node);
		
		creationId += 5;
	}

	PanelNodes::~PanelNodes()
	{
		ed::DestroyEditor(context);

		for (auto n : nodes)
		{
			delete n;
			n = nullptr;
		}
		nodes.clear();

	}

	void PanelNodes::Update(TimeStep ts)
	{
		for (const auto node : nodes)
		{
			node->Update(ts);
		}

		editorLayer->SetDiffuseData(diffuseNode);
		editorLayer->SetNormalsData(normalsNode);
		editorLayer->SetMetallicData(metallicNode);
		editorLayer->SetRoughnessData(roughnessNode);
	}

	void PanelNodes::OnImGuiRender(PanelInspector* panelInspector)
	{
		ed::NodeId selectedId;
		ed::GetSelectedNodes(&selectedId, 1);
		panelInspector->OnImGuiRender(editorLayer->model, editorLayer->dirLight, FindNode(selectedId));

		ImGui::Begin("Node editor");

		if (ImGui::IsWindowHovered())
		{
			if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_RIGHT))
			{
				rightClickedNodeId = ed::GetHoveredNode();
				rightClickedNodeId.Get() != 0 ? showCreationPopup = false : showCreationPopup = true;
			}
			else if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
			{
				showCreationPopup = false;
				showNodePopup = false;
			}
		}

		if (showCreationPopup)
		{
			ShowNodeCreationPopup();
		}

		// NODES WORKSPACE =======================================================
		ed::Begin("");
			DrawNodes(nodes, links);
		ed::End();
		// NODES WORKSPACE =======================================================

		ImGui::End();
	}


	void PanelNodes::DrawNodes(std::vector<Node*>& nodes, std::vector<LinkInfo>& links)
	{
		HandleNodes(nodes);

		for (const auto& n : nodes)
		{
			ed::BeginNode(n->id);
			ed::PushStyleColor(ed::StyleColor_NodeBg, n->nodeColor);
			ImGui::Text(n->name.c_str());
			ImGui::NewLine();

			ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;
			if (ImGui::BeginTable(std::to_string(n->id.Get()).c_str(), 3, flags))
			{
				ImGui::TableNextColumn();

				for (const auto& input : n->inputs)
				{
					ed::BeginPin(input.id, input.kind);
					ImGui::Text(input.name.c_str());
					ed::EndPin();
				}

				ImGui::TableNextColumn();

				n->OnImGuiNodeRender();

				ImGui::TableNextColumn();
				for (const auto& output : n->outputs)
				{
					ed::BeginPin(output.id, output.kind);
					ImGui::Text(output.name.c_str());
					ed::EndPin();
					ImGui::Dummy({ 0,10 });
				}

				ImGui::EndTable();
				ed::PopStyleColor();
			}
			ed::EndNode();
		}

		// Links creation
		HandleLinks(links);

		// Links Drawing
		for (const auto& link : links)
			ed::Link(link.id, link.inputId, link.outputId);

	}

	void PanelNodes::HandleNodes(std::vector<Node*>& nodes)
	{
		if (ed::BeginDelete())
		{
			ed::NodeId id;
			if (ed::QueryDeletedNode(&id))
			{
				if (ed::AcceptDeletedItem())
				{
					Node* node = FindNode(id);
					for (int i = 0; i < links.size(); ++i)
					{
						LinkInfo& link = links[i];

						Pin inputPin = *FindPin(link.inputId);
						Pin outputPin = *FindPin(link.outputId);
						if (outputPin.node->id.Get() == OUTPUT_NODE_ID)
						{
							UpdateOutputNodeData(inputPin, outputPin, true);
							links.erase(links.begin() + i);
						}
					}
					DeleteNode(id);
					ed::DeleteNode(id);
				}
			}
		}
		ed::EndDelete();
	}

	void PanelNodes::HandleLinks(std::vector<LinkInfo>& links)
	{
		if (ed::BeginCreate())
		{
			ed::PinId inputPinId, outputPinId;
			if (ed::QueryNewLink(&inputPinId, &outputPinId))
			{
				if (ed::PinHadAnyLinks(outputPinId))
				{
					ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
					ed::EndCreate();
					return;
				}

				if (inputPinId && outputPinId)
				{
					Pin startPin = *FindPin(inputPinId);
					Pin endPin = *FindPin(outputPinId);

					if (startPin.kind == ed::PinKind::Input)
					{
						std::swap(startPin, endPin);
						std::swap(inputPinId, outputPinId);
					}

					if (!startPin.id.Invalid && !endPin.id.Invalid)
					{
						if (endPin.id == startPin.id)
						{
							ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
						}
						else if (endPin.kind == startPin.kind)
						{
							ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
						}
						else if (endPin.type != startPin.type)
						{
							ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
						}
						
						else if (ed::AcceptNewItem(ImColor(128, 255,128)))
						{
							static int linkId = 100;
							links.push_back({ ed::LinkId(linkId++), inputPinId, outputPinId });
							
							OnLinkCreation(startPin, endPin);
							UpdateOutputNodeData(startPin, endPin, false);
						}
					}
				}
			}
		}
		ed::EndCreate();

		if (ed::BeginDelete())
		{
			ed::LinkId deletedLinkId;
			while (ed::QueryDeletedLink(&deletedLinkId))
			{
				if (ed::AcceptDeletedItem())
				{
					for (int i = 0; i < links.size(); ++i)
					{
						if (links[i].id == deletedLinkId)
						{
							LinkInfo link = links[i];
							Pin inputPin = *FindPin(link.inputId);
							Pin outputPin = *FindPin(link.outputId);
							Node* inputNode = inputPin.node;
							Node* outputNode = outputPin.node;

							UpdateOutputNodeData(inputPin, outputPin, true);

							// TODO: Handle links deletion between normal/current nodes

							for (int j = 0; j < outputPin.links.size(); ++j)
							{
								if (outputPin.links[j].id == deletedLinkId)
								{
									outputPin.links.erase(outputPin.links.begin() + j);
								}
							}

							links.erase(links.begin() + i);

							break;
						}
					}
				}
			}
		}
		ed::EndDelete();
	}

	void PanelNodes::ShowNodeCreationPopup()
	{
		ImGui::OpenPopup("CreationPopup");
		if (ImGui::BeginPopup("CreationPopup"))
		{
			if (ImGui::BeginMenu("Basics"))
			{
				if (ImGui::MenuItem("Flat Color"))
				{
					CreateFlatColorNode("Flat Color", { 1,0,0 });
					showCreationPopup = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Constants"))
			{
				if (ImGui::MenuItem("Vector1"))
				{
					CreateVector1Node("Vector 1");
					showCreationPopup = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Filters"))
			{
				if (ImGui::MenuItem("Normal Map"))
				{
					CreateNormalMapNode("Normal Map");
					showCreationPopup = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Generators"))
			{
				if (ImGui::MenuItem("Circle"))
				{
					CreateCircleNode("Circle");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Noise"))
				{
					CreateNoiseNode("Noise", NoiseType::NORMAL);
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Perlin Noise"))
				{
					CreateNoiseNode("Noise", NoiseType::PERLIN);
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Voronoi"))
				{
					CreateVoronoiNode("Voronoi");
					showCreationPopup = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Operators"))
			{
				if (ImGui::MenuItem("Blend"))
				{
					// TODO: Handle blending between two colors
					CreateBlendNode("Blend");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Clamp"))
				{
					CreateClampNode("Clamp");
					showCreationPopup = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		ImGui::CloseCurrentPopup();
	}

	Node* PanelNodes::FindNode(ed::NodeId id) const
	{
		for (const auto node : nodes)
		{
			if (node->id == id)
				return node;
		}

		return nullptr;
	}

	Pin* PanelNodes::FindPin(ed::PinId id) const
	{
		for (int i = 0; i < nodes.size(); ++i)
		{
			for (auto inputPin : nodes[i]->inputs)
			{
				if (inputPin.id == id)
					return &inputPin;
			}

			for (auto outputPin : nodes[i]->outputs)
			{
				if (outputPin.id == id)
					return &outputPin;
			}
		}

		return nullptr;
	}

	LinkInfo* PanelNodes::FindLink(const ed::LinkId& id) const
	{
		for (auto link : links)
		{
			if (link.id == id)
				return &link;
		}
		return nullptr;
	}

	void PanelNodes::DeleteNode(ed::NodeId id)
	{
		int i = 0;
		for (auto node : nodes)
		{
			if (id == node->id)
			{
				delete node;
				node = nullptr;
				nodes.erase(nodes.begin() + i);
				break;
			}
			i++;
		}
	}

	void PanelNodes::DeleteLink(const ed::LinkId& id) const
	{
		const LinkInfo link = *FindLink(id);
		const Pin output = *FindPin(link.outputId);

		Node* other = output.node;
		if (other->outputType == NodeOutputType::TEXTURE)
		{
			const auto node = (ComputeShaderNode*)other;
			node->BindCoreData();
			node->computeShader->SetUniformVec3f("inputColor", { 1,1,1 });
			node->DispatchCompute(8, 4);
		}

		ed::DeleteLink(id);
	}

	Node* PanelNodes::CreateFlatColorNode(const char* name, const glm::vec3& color)
	{
		ColorNode* node = new ColorNode(creationId++, name, NodeOutputType::FLAT_COLOR, color);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin pin = Pin(creationId++, "Output", PinType::Object, ed::PinKind::Output);
		pin.node = node;
		node->outputs.emplace_back(pin);

		return nodes.back();
	}

	Node* PanelNodes::CreateNoiseNode(const char* name, NoiseType noiseType)
	{
		NoiseNode* node = new NoiseNode(creationId++, name, NodeOutputType::TEXTURE, noiseType);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "Input", PinType::Object, ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin output = Pin(creationId++, "Output", PinType::Object, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return nodes.back();
	}

	Node* PanelNodes::CreateVoronoiNode(const char* name)
	{
		VoronoiNode* node = new VoronoiNode(creationId++, name, NodeOutputType::TEXTURE);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin output = Pin(creationId++, "Output", PinType::Object, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return node;
	}

	Node* PanelNodes::CreateVector1Node(const char* name)
	{
		Vec1Node* node = new Vec1Node(creationId++, name, NodeOutputType::VEC1	);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin output = Pin(creationId++, "Output", PinType::Float, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return node;
	}

	Node* PanelNodes::CreateNormalMapNode(const char* name)
	{
		NormalMapNode* node = new NormalMapNode(creationId++, name, NodeOutputType::TEXTURE);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "Input", PinType::Object, ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin output = Pin(creationId++, "Output", PinType::Object, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return node;
	}

	Node* PanelNodes::CreateCircleNode(const char* name)
	{
		CircleNode* node = new CircleNode(creationId++, name, NodeOutputType::TEXTURE);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		//Pin input = Pin(creationId++, "Input", PinType::Object, ed::PinKind::Input);
		//input.node = node;
		//node->inputs.emplace_back(input);

		Pin output = Pin(creationId++, "Output", PinType::Object, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return nullptr;
	}

	Node* PanelNodes::CreateBlendNode(const char* name)
	{
		BlendNode* node = new BlendNode(creationId++, name, NodeOutputType::TEXTURE);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "O", PinType::Object, ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin input2 = Pin(creationId++, "O", PinType::Object, ed::PinKind::Input);
		input2.node = node;
		node->inputs.emplace_back(input2);

		Pin output = Pin(creationId++, "Output", PinType::Object, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return node;
	}

	Node* PanelNodes::CreateClampNode(const char* name)
	{
		ClampNode* node = new ClampNode(creationId++, name, NodeOutputType::TEXTURE);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "O", PinType::Object, ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin output = Pin(creationId++, "O", PinType::Object, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return nullptr;
	}

	void PanelNodes::OnLinkCreation(Pin& startPin, Pin& endPin)
	{
		if (endPin.node->outputType == NodeOutputType::TEXTURE)
		{
			if (startPin.node->outputType == NodeOutputType::FLAT_COLOR)
			{
				if (endPin.node->type == NodeType::CLAMP)
				{
					const auto n = (ClampNode*)endPin.node;
					const auto inNode = (ColorNode*)startPin.node;
					n->SetInputColor(inNode->color);
				}
				else
				{
					const auto n = (ComputeShaderNode*)endPin.node;
					const auto inNode = (ColorNode*)startPin.node;
					n->BindCoreData();
					n->computeShader->SetUniformVec3f("inputColor", inNode->color);
					n->DispatchCompute(8, 4);
				}
			}
			else if (startPin.node->outputType == NodeOutputType::TEXTURE)
			{
				const auto inNode = (ComputeShaderNode*)startPin.node;
				// TODO: Instead of check for the normal map, check for texture and all compute shaders have the uniform inputTexture?
				if (endPin.node->type == NodeType::NORMAL_MAP)
				{
					const auto n = (NormalMapNode*)endPin.node;
					*n->inputTexture = *inNode->texture;
				}
				else if (endPin.node->type == NodeType::BLEND)
				{
					const auto n = (BlendNode*)endPin.node;
					n->inputs[0].id.Get() < endPin.id.Get() ? *n->tex2 = *inNode->texture : *n->tex1 = *inNode->texture;
				}
				else if (endPin.node->type == NodeType::CLAMP)
				{
					const auto n = (ClampNode*)endPin.node;
					n->SetInputTexture(inNode->texture);
				}
			}
		}


	}

	void PanelNodes::UpdateOutputNodeData(Pin& startPin, Pin& endPin, bool resetData)
	{
		if (endPin.node->id != ed::NodeId(OUTPUT_NODE_ID))
			return;
		
		switch (endPin.id.Get())
		{
			// Albedo
			case OUTPUT_ALBEDO_PIN_ID:
			{
				if (!resetData)
				{
					editorLayer->SetDiffuseData(startPin.node);
					diffuseNode = startPin.node;
				}
				else
				{
					editorLayer->SetDiffuseData(nullptr);
					diffuseNode = nullptr;
				}
				break;
			}
			// Normals
			case OUTPUT_NORMALS_PIN_ID:
			{
				if (!resetData)
				{
					editorLayer->SetNormalsData(startPin.node);
					normalsNode = startPin.node;
				}
				else
				{
					editorLayer->SetNormalsData(nullptr);
					normalsNode = nullptr;
				}
				break;
			}
			// Metallic
			case OUTPUT_METALLIC_PIN_ID:
			{

				break;
			}
			// Roughness
			case OUTPUT_ROUGHNESS_PIN_ID:
			{
				editorLayer->SetRoughnessData(startPin.node);
				roughnessNode = startPin.node;
				break;
			}

			default:
				break;

		}
	}
}