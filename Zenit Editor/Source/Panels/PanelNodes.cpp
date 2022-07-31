#include "PanelNodes.h"
#include "PanelInspector.h"

#include "Nodes/ColorNode.h"
#include "Nodes/ComputeShaderNode.h"
#include "Nodes/PerlinNoiseNode.h"
#include "Nodes/VoronoiNode.h"
#include "Nodes/Constants/Vec1Node.h"

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

		Pin metallic = Pin(OUTPUT_METALLIC_PIN_ID, "Metallic", PinType::Float, ed::PinKind::Input);
		metallic.node = node;
		node->inputs.emplace_back(metallic);

		Pin roughness = Pin(OUTPUT_ROUGHNESS_PIN_ID, "Roughness", PinType::Float, ed::PinKind::Input);
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
		panelInspector->OnImGuiRender(editorLayer->model, editorLayer->dirLight, FindNode(selectedNodeId));

		ImGui::Begin("Node editor");

		if (ImGui::IsWindowHovered())
		{
			if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_RIGHT))
			{
				rightClickedNodeId = ed::GetHoveredNode();
				rightClickedNodeId.Get() != 0 ? showNodePopup = true : showCreationPopup = true;
			}
			else if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
			{
				if (ed::NodeId id = ed::GetHoveredNode())
				{
					selectedNodeId = id;
				}
				else
				{
					selectedNodeId = 0;
				}
				showCreationPopup = false;
				showNodePopup = false;
			}
		}


		if (showCreationPopup)
		{
			ShowNodeCreationPopup();
		}

		if (showNodePopup)
		{
			ShowNodeOptionsPopup();
		}

		if (Input::GetInstance()->IsKeyPressed(KEY_DELETE))
		{
			if (selectedNodeId.Get() != 0)
			{
				ed::DeleteNode(selectedNodeId);
				DeleteNode(selectedNodeId);
				selectedNodeId = 0;
			}
			else if (const ed::LinkId id = ed::GetHoveredLink())
			{
				ed::DeleteLink(id);
			}
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
		for (auto n : nodes)
		{
			ed::BeginNode(n->id);
			ed::PushStyleColor(ed::StyleColor_NodeBg, n->nodeColor);
			ImGui::Text(n->name.c_str());
			ImGui::NewLine();

			ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;
			if (ImGui::BeginTable(std::to_string(n->id.Get()).c_str(), 3, flags))
			{
				ImGui::TableNextColumn();

				for (auto& input : n->inputs)
				{
					ed::BeginPin(input.id, input.kind);
					ImGui::Text(input.name.c_str());
					ed::EndPin();
				}

				ImGui::TableNextColumn();

				n->OnImGuiNodeRender();

				ImGui::TableNextColumn();
				for (auto& output : n->outputs)
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

							if (endPin.node->outputType == NodeOutputType::TEXTURE)
							{
								const auto n = (ComputeShaderNode*)endPin.node;
								n->BindCoreData();
								if (startPin.node->outputType == NodeOutputType::FLAT_COLOR)
								{
									const auto inNode = (ColorNode*)startPin.node;
									n->computeShader->SetUniformVec3f("inputColor", inNode->color);
								}
								n->DispatchCompute(8, 4);
							}

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
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Generators"))
			{
				if (ImGui::MenuItem("Noise"))
				{
					CreateNoiseNode("Noise");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Voronoi"))
				{
					CreateVoronoiNode("Voronoi");
					showCreationPopup = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		ImGui::CloseCurrentPopup();
	}

	void PanelNodes::ShowNodeOptionsPopup()
	{
		ImGui::OpenPopup("NodePopup");
		if (ImGui::BeginPopup("NodePopup"))
		{
			if (ImGui::BeginMenu("Set as Output"))
			{
				if (rightClickedNodeId.Get() != 0)
				{
					Node* node = FindNode(rightClickedNodeId);

					if (ImGui::MenuItem("Diffuse"))
					{
						if (editorLayer->SetDiffuseData(node))
							rightClickedNodeId = 0;

						showNodePopup = false;
					}
					else if (ImGui::MenuItem("Normals"))
					{
						editorLayer->normalsOutput = node;
						showNodePopup = false;
					}
					else if (ImGui::MenuItem("Metallic"))
					{
						editorLayer->metallicOutput = node;
						showNodePopup = false;
					}
					else if (ImGui::MenuItem("Roughness"))
					{
						editorLayer->roughnessOutput = node;
						showNodePopup = false;
					}
					else if (ImGui::MenuItem("Ambient Occlusion"))
					{
						editorLayer->aoOutput = node;
						showNodePopup = false;
					}
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

	Node* PanelNodes::CreateNoiseNode(const char* name)
	{
		PerlinNoiseNode* node = new PerlinNoiseNode(creationId++, name, NodeOutputType::TEXTURE);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "Input", PinType::Object, ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin output = Pin(creationId++, "Output", PinType::Object, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		node->computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/noise.shader");
		node->texture = std::make_unique<Texture2D>(nullptr, 512, 512);

		node->BindCoreData();
		node->computeShader->SetUniformVec3f("inputColor", { 1,1,1 });
		node->computeShader->SetUniform1f("size", node->seed);
		node->DispatchCompute(8, 4);

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

		node->computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/voronoi.shader");
		node->texture = std::make_unique<Texture2D>(nullptr, 512, 512);

		node->BindCoreData();
		//node->computeShader->SetUniformVec3f("inputColor", { 1,1,1 });
		node->DispatchCompute(8, 4);

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

	void PanelNodes::UpdateOutputNodeData(Pin& startPin, Pin& endPin, bool resetData)
	{
		if (endPin.node->id == ed::NodeId(OUTPUT_NODE_ID))
		{
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

}