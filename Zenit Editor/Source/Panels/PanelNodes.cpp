#include "Zenit.h"

#include "PanelNodes.h"
#include "PanelInspector.h"

#include "Nodes/ColorNode.h"
#include "Nodes/ComputeShaderNode.h"
#include "Nodes/NoiseNode.h"
#include "Nodes/VoronoiNode.h"
#include "Nodes/CircleNode.h"

#include "Nodes/Filters/NormalMapNode.h"
#include "Nodes/Filters/TwirlNode.h"

#include "Nodes/Constants/Vec1Node.h"

#include "Nodes/Operators/BlendNode.h"
#include "Nodes/Operators/ClampNode.h"
#include "Nodes/Operators/MaxMinNode.h"

#include "EditorLayer.h"

#include <ImGui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

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
		context = reinterpret_cast<ax::NodeEditor::Detail::EditorContext*>(ed::CreateEditor(&config));
		ed::SetCurrentEditor(reinterpret_cast<ax::NodeEditor::EditorContext*>(context));


		Node* node = new Node(OUTPUT_NODE_ID, "PBR", NodeOutputType::NONE);

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
		ed::DestroyEditor(reinterpret_cast<ax::NodeEditor::EditorContext*>(context));

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

		if (!hovered)
			return;

		//static bool startedDragging = false;
		//if (!startedDragging && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		//{
		//	startedDragging = true;
		//}
		//if (startedDragging && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			ImVec4 bounds = context->GetSelectionBounds().ToVec4();/* + context->GetViewRect().ToVec4()*/;
			lastSelectionBounds = { bounds.x, bounds.y, bounds.z, bounds.w };
			ZN_CORE_TRACE("{0}, {1}, {2}, {3}", lastSelectionBounds.Min.x, lastSelectionBounds.Min.y, lastSelectionBounds.Max.x, lastSelectionBounds.Max.y);
			//startedDragging = false;
		}
		
		

		//editorLayer->SetDiffuseData(diffuseNode);
		//editorLayer->SetNormalsData(normalsNode);
		//editorLayer->SetMetallicData(metallicNode);
		//editorLayer->SetRoughnessData(roughnessNode);
	}

	void PanelNodes::OnImGuiRender(PanelInspector* panelInspector)
	{
		ed::NodeId selectedId;
		ed::GetSelectedNodes(&selectedId, 1);
		panelInspector->OnImGuiRender(editorLayer->currentModel, editorLayer->dirLight, FindNode(selectedId));

		ImGui::Begin("Node editor");
		
		panelSize = ImGui::GetWindowSize();
		hovered = ImGui::IsWindowHovered();

		if (ImGui::IsWindowHovered())
		{
			if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_RIGHT))
			{
				if (ed::GetSelectedObjectCount() > 0)
				{
					showGroupPopup = true;
				}
				else
				{
					rightClickedNodeId = ed::GetHoveredNode();
					rightClickedNodeId.Get() != 0 ? showCreationPopup = false : showCreationPopup = true;
				}
			}
			else if (ed::GetSelectedObjectCount() > 0 && Input::GetInstance()->IsKeyPressed(KEY_SPACE))
			{
				showGroupPopup = true;
			}
			else if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
			{
				showCreationPopup = false;
				showGroupPopup = false;
			}
		}

		if (showCreationPopup)
		{
			ShowNodeCreationPopup();
		}
		else if (showGroupPopup)
		{
			ImGui::OpenPopup("GroupPopup");
			if (ImGui::BeginPopup("GroupPopup"))
			{
				if (ImGui::MenuItem("Group"))
				{
					CreateGroupNode("Group");
					showGroupPopup = false;
				}
				ImGui::EndPopup();
			}
			ImGui::CloseCurrentPopup();
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

		for (auto n : nodes)
		{
			if (n->type == NodeType::COMMENT)
			{
				DrawGroupNode(n);
			}
			else
			{
				ed::BeginNode(n->id);
				ImGui::PushID(n->id.AsPointer());
				ed::PushStyleColor(ed::StyleColor_NodeBg, n->nodeColor);
				
				int width = ImGui::CalcTextSize(n->name.c_str()).x + 8;

				ImGui::PushItemWidth(width);

				ed::NodeId selectedNodeId;
				ed::GetSelectedNodes(&selectedNodeId, 1);
				if (selectedNodeId == n->id && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					n->changeName = true;
				}
				else if (selectedNodeId != n->id)
				{
					n->changeName = false;
				}

				if (n->changeName)
				{
					ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
					if (ImGui::InputText("", &n->name, flags))
					{
						n->changeName = false;
					}
				}
				else
				{
					ImGui::TextUnformatted(n->name.c_str());
				}
				ImGui::PopItemWidth();

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
				ImGui::PopID();
				ed::EndNode();
			}

		}

		// Links creation
		HandleLinks(links);

		// Links Drawing
		for (const auto& link : links)
			ed::Link(link.id, link.inputId, link.outputId);

	}

	void PanelNodes::DrawGroupNode(Node* node)
	{		
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);
		ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
		ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
		
		ed::BeginNode(node->id);
		ImGui::PushID(node->id.AsPointer());

		ImGui::PushItemWidth(node->size.x);
		//static bool changeName = false;
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			node->changeName = true;
		}

		if (node->changeName)
		{
			ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
			if (ImGui::InputText("", &node->name, flags))
			{
				node->changeName = false;
			}
		}
		else
		{
			ImGui::TextUnformatted(node->name.c_str());
		}
		ImGui::PopItemWidth();
		
		ed::Group(node->size);

		ImGui::PopID();
		ed::EndNode();
		ed::PopStyleColor(2);
		ImGui::PopStyleVar();

		if (ed::BeginGroupHint(node->id))
		{
			ImVec2 min = ed::GetGroupMin();
			auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

			glm::vec2 glmMin = { min.x, min.y };
			glm::vec2 glmPos = glmMin - glm::vec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4);
			ImVec2 pos = { glmPos.x, glmPos.y };

			//ed::PushStyleColor(ed::StyleColor_NodeBg, node->nodeColor);
			ImGui::SetCursorScreenPos(pos);
			ImGui::BeginGroup();
			ImGui::TextUnformatted(node->name.c_str());
			ImGui::EndGroup();

			auto drawList = ed::GetHintBackgroundDrawList();

			ImRect hintBounds = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax() };
			ImRect hintFrameBounds = hintBounds;
			int x = 8;
			int y = 4;
			hintFrameBounds.Min.x -= x;
			hintFrameBounds.Min.y -= y;
			hintFrameBounds.Max.x += x;
			hintFrameBounds.Max.y += y;


			drawList->AddRectFilled(
				hintFrameBounds.GetTL(),
				hintFrameBounds.GetBR(),
				IM_COL32(180, 180, 180, 10), 1.0f);

			drawList->AddRect(
				hintFrameBounds.GetTL(),
				hintFrameBounds.GetBR(),
				IM_COL32(180, 180, 180, 20), 1.0f);

		}
		ed::EndGroupHint();
	}

	void PanelNodes::HandleNodes(std::vector<Node*>& nodes)
	{
		if (ed::BeginDelete())
		{
			// Avoid undesired deletes
			if (GImGui->ActiveId != 0)
			{
				ed::EndDelete();
				return;
			}

			ed::NodeId id;
			if (ed::QueryDeletedNode(&id))
			{
				if (ed::AcceptDeletedItem(false))
				{
					if (id.Get() == OUTPUT_NODE_ID)
					{
						ed::EndDelete();
						return;
					}

					Node* deletedNode = FindNode(id);
					std::vector<int> toErase;
					toErase.resize(links.size());
					toErase = { -1 };

					auto& it = links.begin();
					while (it != links.end())
					{
						LinkInfo& currentLink = *(it);

						Pin* inputPin = FindPin(currentLink.inputId);
						Pin* outputPin = FindPin(currentLink.outputId);

						// TODO: Removing here leads to problems!!!
						if (outputPin->node == deletedNode)
						{
							UpdateNode(inputPin, outputPin, true);
							UpdateOutputNodeData(*inputPin, *outputPin, true);
							it = links.erase(it);

						}
						else if (inputPin->node == deletedNode)
						{
							UpdateNode(inputPin, outputPin, true);
							UpdateOutputNodeData(*inputPin, *outputPin, true);
							it = links.erase(it);
						}
						else
						{
							++it;
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
					Pin* startPin = FindPin(inputPinId);
					Pin* endPin = FindPin(outputPinId);

					if (startPin->id.Get() < 0 && endPin->id.Get() < 0)
						return;

					if (startPin->kind == ed::PinKind::Input)
					{
						std::swap(startPin, endPin);
						std::swap(inputPinId, outputPinId);
					}

					if (endPin->id == startPin->id)
					{
						ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
					}
					else if (endPin->kind == startPin->kind)
					{
						ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
					}
					else if (endPin->type != startPin->type)
					{
						ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
					}
					else if (ed::AcceptNewItem(ImColor(128, 255,128)))
					{
						LinkInfo link = LinkInfo(ed::LinkId(linkCreationId++), inputPinId, outputPinId);
						links.push_back(link);

						UpdateNode(startPin, endPin, false);
						UpdateOutputNodeData(*startPin, *endPin, false);
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
					LinkInfo* deletedLink = FindLink(deletedLinkId);

					for (int i = 0; i < links.size(); ++i)
					{
						if (links[i].id == deletedLinkId)
						{
							LinkInfo link = links[i];
							Pin* inputPin = FindPin(link.inputId);
							Pin* outputPin = FindPin(link.outputId);

							UpdateOutputNodeData(*inputPin, *outputPin, true);

							// TODO: Handle links deletion between normal/current nodes
							UpdateNode(inputPin, outputPin, true);

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
				else if (ImGui::MenuItem("Twirl"))
				{
					CreateTwirlNode("Twirl");
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
					CreateBlendNode("Blend");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Clamp"))
				{
					CreateClampNode("Clamp");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Max"))
				{
					CreateMaxNode("Max");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Min"))
				{
					CreateMinNode("Min");
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
		for (const auto& node : nodes)
		{
			if (node->id == id)
				return node;
		}

		return nullptr;
	}

	Pin* PanelNodes::FindPin(ed::PinId id)
	{
		for (int i = 0; i < nodes.size(); ++i)
		{
			for (auto& inputPin : nodes[i]->inputs)
			{
				if (inputPin.id == id)
					return &inputPin;
			}

			for (auto& outputPin : nodes[i]->outputs)
			{
				if (outputPin.id == id)
					return &outputPin;
			}
		}

		return &incorrectPin;
	}

	LinkInfo* PanelNodes::FindLink(const ed::LinkId& id)
	{
		for (auto& link : links)
		{
			if (link.id == id)
				return &link;
		}

		return &incorrectLink;
	}

	void PanelNodes::DeleteNode(ed::NodeId id)
	{
		for (int i = 0; i < nodes.size(); ++i)
		{
			if (id == nodes[i]->id)
			{
				delete nodes[i];
				nodes[i] = nullptr;
				nodes.erase(nodes.begin() + i);
				break;
			}
		}
	}

	void PanelNodes::DeleteLink(const ed::LinkId& id)
	{
		const LinkInfo* link = FindLink(id);
		const Pin* output = FindPin(link->outputId);

		ComputeShaderNode* other = (ComputeShaderNode*)output->node;
		//if (other->outputType == NodeOutputType::TEXTURE)
		//{
			other->BindCoreData();
			other->computeShader->SetUniformVec3f("inputColor", { 1,1,1 });
			other->DispatchCompute(1, 1);
		//}

		ed::DeleteLink(id);
	}

	Node* PanelNodes::CreateFlatColorNode(const char* name, const glm::vec3& color)
	{
		ColorNode* node = new ColorNode(creationId++, name, NodeOutputType::TEXTURE, color);
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

	Node* PanelNodes::CreateTwirlNode(const char* name)
	{
		TwirlNode* node = new TwirlNode(creationId++, name, NodeOutputType::TEXTURE);
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

		return node;
	}

	Node* PanelNodes::CreateMinNode(const char* name)
	{
		MaxMinNode* node = new MaxMinNode(creationId++, name, NodeOutputType::TEXTURE, false);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "O", PinType::Object, ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin input2 = Pin(creationId++, "O", PinType::Object, ed::PinKind::Input);
		input2.node = node;
		node->inputs.emplace_back(input2);

		Pin output = Pin(creationId++, "O", PinType::Object, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);


		return node;
	}

	Node* PanelNodes::CreateMaxNode(const char* name)
	{
		MaxMinNode* node = new MaxMinNode(creationId++, name, NodeOutputType::TEXTURE, true);
		node->size = { 5,5 };
		//node->pos = ImGui::GetMousePos();
		//ed::SetNodePosition(node->id, node->pos);
		//ImGui::SetCursorPos();
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "O", PinType::Object, ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin input2 = Pin(creationId++, "O", PinType::Object, ed::PinKind::Input);
		input2.node = node;
		node->inputs.emplace_back(input2);

		Pin output = Pin(creationId++, "O", PinType::Object, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return node;
	}

	void PanelNodes::CreateGroupNode(const char* name)
	{
		Node* node = new Node(creationId++ , name, NodeOutputType::NONE);
		node->type = NodeType::COMMENT;

		ImVec2 padding = { 16, 35 };
		node->pos = lastSelectionBounds.GetTL() - padding;
		ed::SetNodePosition(node->id, node->pos);

		node->size = lastSelectionBounds.GetSize() + padding;
		nodes.emplace_back(node);

		lastSelectionBounds = { 0,0,0,0 };
		ed::ClearSelection();
	}

	void PanelNodes::UpdateNode(Pin* startPin, Pin* endPin, bool resetData)
	{
		//if (endPin->node->outputType == NodeOutputType::TEXTURE)
		{
			//if (startPin->node->outputType == NodeOutputType::TEXTURE)
			{
				const auto inNode = (ComputeShaderNode*)startPin->node;
				// TODO: Instead of check for the normal map, check for texture and all compute shaders have the uniform inputTexture?
				// If you want an inputTexture, just use a multiply node
				switch (endPin->node->type)
				{
					case NodeType::NORMAL_MAP:
					{
						const auto n = (NormalMapNode*)endPin->node;
						resetData ? n->SetTexture(editorLayer->white) : n->SetTexture(inNode->texture);
						break;
					}
					case NodeType::TWIRL:
					{
						const auto n = (TwirlNode*)endPin->node;
						resetData ? n->SetTexture(editorLayer->white.get()) : n->SetTexture(inNode->texture.get());
						break;
					}
					case NodeType::BLEND:
					{
						const auto n = (BlendNode*)endPin->node;

						Texture2D* tex = nullptr;
						resetData ? tex = editorLayer->white.get() : tex = inNode->texture.get();

						n->inputs[0].id.Get() < endPin->id.Get() ? n->SetSecondTexture(tex) : n->SetFirstTexture(tex);

						break;
					}
					case NodeType::CLAMP:
					{
						const auto n = (ClampNode*)endPin->node;
						resetData ? n->SetInputTexture(editorLayer->white.get()) : n->SetInputTexture(inNode->texture.get());
						break;
					}
					case NodeType::MAX:
					{
						const auto n = (MaxMinNode*)endPin->node;

						Texture2D* tex = nullptr;
						resetData ? tex = editorLayer->white.get() : tex = inNode->texture.get();

						n->inputs[0].id.Get() < endPin->id.Get() ? n->SetSecondTexture(tex) : n->SetFirstTexture(tex);

						break;
					}
					case NodeType::MIN:
					{
						const auto n = (MaxMinNode*)endPin->node;

						Texture2D* tex = nullptr;
						resetData ? tex = editorLayer->white.get() : tex = inNode->texture.get();

						n->inputs[0].id.Get() < endPin->id.Get() ? n->SetSecondTexture(tex) : n->SetFirstTexture(tex);

						break;
					}
					

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
				if (!resetData)
				{
					editorLayer->SetMetallicData(startPin.node);
					metallicNode = startPin.node;
				}
				else
				{
					editorLayer->SetMetallicData(nullptr);
					metallicNode = nullptr;
				}
				break;
			}
			// Roughness
			case OUTPUT_ROUGHNESS_PIN_ID:
			{
				if (!resetData)
				{
					editorLayer->SetRoughnessData(startPin.node);
					roughnessNode = startPin.node;
				}
				else
				{
					editorLayer->SetRoughnessData(nullptr);
					roughnessNode = nullptr;
				}
				break;
			}

			default:
				break;

		}
	}
}