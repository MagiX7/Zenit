#include "Zenit.h"

#include "PanelNodes.h"

#include "Nodes/Node.h"
#include "Nodes/ColorNode.h"
#include "Nodes/GradientNode.h"

#include "Nodes/Generators/CircleNode.h"
#include "Nodes/Generators/CheckersNode.h"
#include "Nodes/Generators/NoiseNode.h"
#include "Nodes/Generators/VoronoiNode.h"
#include "Nodes/Generators/VoronoiNode.h"
#include "Nodes/Generators/KifsFractalNode.h"

#include "Nodes/Filters/NormalMapNode.h"
#include "Nodes/Filters/EdgeDetectorNode.h"
#include "Nodes/Filters/TwirlNode.h"
#include "Nodes/Filters/InvertNode.h"

#include "Nodes/Operators/BlendNode.h"
#include "Nodes/Operators/ClampNode.h"
#include "Nodes/Operators/MaxMinNode.h"
#include "Nodes/Operators/SingleInstructionNode.h"

#include "Nodes/TransformNode.h"
#include "Nodes/GroupNode.h"

#include "EditorLayer.h"
#include "../Helpers/NodeHelpers.h"

#include <ImGui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#define OUTPUT_NODE_ID 1
#define OUTPUT_ALBEDO_PIN_ID 2
#define OUTPUT_NORMALS_PIN_ID 3
#define OUTPUT_METALLIC_PIN_ID 4
#define OUTPUT_ROUGHNESS_PIN_ID 5

namespace Zenit {

	std::vector<Node*> PanelNodes::nodes = {};

	PanelNodes::PanelNodes(EditorLayer* edLayer) : editorLayer(edLayer)
	{
		config = ed::Config();
		config.SettingsFile = "";
		config.UserPointer = this;

		config.NavigateButtonIndex = 2;
		context = reinterpret_cast<ax::NodeEditor::Detail::EditorContext*>(ed::CreateEditor(&config));
		ed::SetCurrentEditor(reinterpret_cast<ax::NodeEditor::EditorContext*>(context));

		CreateFinalOutputNode();

		nodeBgTexture = std::make_shared<Texture2D>("Settings/NodeBackground.png");
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

		lastSelectionBounds = context->GetSelectionBounds();
	}

	void PanelNodes::OnImGuiRender()
	{
		ImGui::Begin("Node editor");

		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();
		hovered = ImGui::IsMouseHoveringRect(pos, pos + size);

		if (hovered)
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
		}
		else 
		{
			showCreationPopup = showGroupPopup = false;
		}

		// IsWindowHovered() returns false if an element is on top of the window and the mouse is hovering that element on top
		if (ImGui::IsWindowHovered() && Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
		{
			showCreationPopup = showGroupPopup = false;
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
			if (n->type == NodeType::GROUP)
			{
				DrawGroupNode(n);
				continue;
			}
			
			ed::PushStyleVar(ax::NodeEditor::StyleVar_NodeRounding, 6.0f);
			ed::BeginNode(n->id);
			
			if (repositionNodes)
				ed::SetNodePosition(n->id, n->pos);

			ImGui::PushID(n->id.AsPointer());

			int width = ImGui::CalcTextSize(n->name.c_str()).x + 8;
			int height = ImGui::CalcTextSize(n->name.c_str()).y + 8;

			ImGui::PushItemWidth(width);

			ed::NodeId selectedNodeId;
			ed::GetSelectedNodes(&selectedNodeId, 1);
			if (selectedNodeId == n->id && ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
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

			ImVec2 titleMin = ImGui::GetItemRectMin();
			ImVec2 titleMax = ImGui::GetItemRectMax();
			ImRect titleRect = { titleMin, titleMax };

			ImGui::Dummy({ 0, 3 });

			ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;
			if (ImGui::BeginTable(std::to_string(n->id.Get()).c_str(), 3, flags))
			{
				ImGui::TableNextColumn();
				for (const auto& input : n->inputs)
				{
					ed::BeginPin(input.id, input.kind);
					ImGui::Text(input.name.c_str());
					ed::EndPin();

					auto rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
					ImVec2 leftCenter = rect.GetCenter();
					leftCenter.x -= rect.GetWidth() / 2;
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
			}
			ImGui::PopID();

			ed::EndNode();

			// Draw Header
			ImVec2 bgMax = ImGui::GetItemRectMax();
			ImVec2 bgMin = ImGui::GetItemRectMin();

			if ((bgMax.x > bgMin.x) && (bgMax.y > bgMin.y))
			{
				float alpha = (int)ImGui::GetStyle().Alpha * 255;
				auto nodeDrawList = ed::GetNodeBackgroundDrawList(n->id);

				auto headerColor = IM_COL32(0, 0, 0, alpha) | (n->headerColor & IM_COL32(255, 255, 255, 0));

				ImVec2 headerMin = bgMin;
				ImVec2 headerMax = bgMax;
				headerMax.y = titleMax.y + 3;

				auto uv = ImVec2(
					(headerMax.x - headerMin.x) / (float)(4.0f * nodeBgTexture->GetWidth()),
					(headerMax.y - headerMin.y) / (float)(4.0f * nodeBgTexture->GetHeight()));

				nodeDrawList->AddImageRounded((void*)nodeBgTexture->GetId(), headerMin, headerMax,
					ImVec2(0,0), uv + ImVec2(0.5,0.5),
					headerColor, ed::GetStyle().NodeRounding, 1 | 2);
					
			}

			// Pop border rounding
			ed::PopStyleVar();
		}

		repositionNodes = false;

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
		ed::PushStyleVar(ax::NodeEditor::StyleVar_NodeRounding, 6.0f);
		
		ed::BeginNode(node->id);

		if (repositionNodes)
			ed::SetNodePosition(node->id, node->pos);

		ImGui::PushID(node->id.AsPointer());

		ImGui::PushItemWidth(node->size.x);

		ed::NodeId selectedNodeId;
		ed::GetSelectedNodes(&selectedNodeId, 1);
		if (selectedNodeId == node->id && ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			node->changeName = true;
		}
		else if (selectedNodeId != node->id)
		{
			node->changeName = false;
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
		ed::PopStyleVar();

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
						ed::SetNodePosition(OUTPUT_NODE_ID, ed::GetNodePosition(OUTPUT_NODE_ID));
						return;
					}

					Node* deletedNode = NodeHelpers::FindNode(id, nodes);
					std::vector<int> toErase;
					toErase.resize(links.size());
					toErase = { -1 };

					auto& it = links.begin();
					while (it != links.end())
					{
						LinkInfo& currentLink = *(it);
						
						Pin* inputPin = NodeHelpers::FindPin(currentLink.inputId, nodes);
						Pin* outputPin = NodeHelpers::FindPin(currentLink.outputId, nodes);

						if (outputPin->node == deletedNode)
						{
							UpdateNode(inputPin, outputPin, currentLink, true);
							UpdateOutputNodeData(*inputPin, *outputPin, true);
							it = links.erase(it);
						}
						else if (inputPin->node == deletedNode)
						{
							UpdateNode(inputPin, outputPin, currentLink, true);
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
					Pin* startPin = NodeHelpers::FindPin(inputPinId, nodes);
					Pin* endPin = NodeHelpers::FindPin(outputPinId, nodes);

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
					else if (ed::AcceptNewItem(ImColor(128, 255,128)))
					{
						LinkInfo link = LinkInfo(ed::LinkId(linkCreationId++), inputPinId, outputPinId);
						links.push_back(link);

						// Check if the next node id is already present
						auto it = std::find(startPin->node->nextNodesIds.begin(), startPin->node->nextNodesIds.end(), endPin->node->id);
						if (it == startPin->node->nextNodesIds.end())
							startPin->node->nextNodesIds.emplace_back(endPin->node->id);

						UpdateNode(startPin, endPin, link, false);
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
					const LinkInfo& deletedLink = NodeHelpers::FindLink(deletedLinkId, links);
					
					for (int i = 0; i < links.size(); ++i)
					{
						if (links[i].id == deletedLinkId)
						{
							LinkInfo link = links[i];
							Pin* inputPin = NodeHelpers::FindPin(link.inputId, nodes);
							Pin* outputPin = NodeHelpers::FindPin(link.outputId, nodes);

							UpdateOutputNodeData(*inputPin, *outputPin, true);
							UpdateNode(inputPin, outputPin, deletedLink, true);

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
				else if (ImGui::MenuItem("Gradient"))
				{
					CreateFilterNode<GradientNode>("Gradient");
					showCreationPopup = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Filters"))
			{ 
				if (ImGui::MenuItem("Normal Map"))
				{
					CreateFilterNode<NormalMapNode>("Normal Map");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Edge Detector"))
				{
					CreateFilterNode<EdgeDetectorNode>("Edge Detector");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Twirl"))
				{
					CreateFilterNode<TwirlNode>("Twirl");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Invert"))
				{
					CreateFilterNode<InvertNode>("Invert");
					showCreationPopup = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Generators"))
			{
				if (ImGui::MenuItem("Circle"))
				{
					CreateGeneratorNode<CircleNode>("Circle");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Checkers"))
				{
					CreateGeneratorNode<CheckersNode>("Checkers");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("White Noise"))
				{
					CreateGeneratorNode<NoiseNode>("White Noise", NoiseType::WHITE);
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("FBM"))
				{
					CreateGeneratorNode<NoiseNode>("FBM", NoiseType::FBM);
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Gradient Noise"))
				{
					CreateGeneratorNode<NoiseNode>("Gradient Noise", NoiseType::GRADIENT);
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Voronoi"))
				{
					CreateGeneratorNode<VoronoiNode>("Voronoi");
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("KIFS Fractal"))
				{
					CreateGeneratorNode<KifsFractalNode>("KIFS Fractal");
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
					CreateMaxMinNode("Max", true);
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Min"))
				{
					CreateMaxMinNode("Min", false);
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Pow"))
				{
					CreateSingleInstructionNode("Pow", SingleInstructionType::POW);
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Add"))
				{
					CreateSingleInstructionNode("Add", SingleInstructionType::ADD);
					showCreationPopup = false;
				}
				else if (ImGui::MenuItem("Subtract"))
				{
					CreateSingleInstructionNode("Subtract", SingleInstructionType::SUBTRACT);
					showCreationPopup = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Transform"))
			{
				if (ImGui::MenuItem("Transform 2D"))
				{
					// Despite it is not a filter, it is created the same way
					CreateFilterNode<TransformNode>("Transform 2D");
					showCreationPopup = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		ImGui::CloseCurrentPopup();
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
		const LinkInfo& link = NodeHelpers::FindLink(id, links);
		const Pin* output = NodeHelpers::FindPin(link.outputId, nodes);

		Node* other = (Node*)output->node;
		other->BindCoreData();
		other->computeShader->SetUniformVec3f("inputColor", { 1,1,1 });
		other->DispatchCompute(1, 1);

		ed::DeleteLink(id);
	}

	ColorNode* PanelNodes::CreateFlatColorNode(const char* name, const glm::vec3& color)
	{
		ColorNode* node = new ColorNode(creationId++, name, color);
		node->size = { 5,5 };
		nodes.emplace_back(node);
		node->headerColor = FLAT_COLOR_NODE_HEADER_COLOR;

		Pin pin = Pin(creationId++, "  O  ", ed::PinKind::Output);
		pin.node = node;
		node->outputs.emplace_back(pin);

		return node;
	}

	Node* PanelNodes::CreateBlendNode(const char* name)
	{
		BlendNode* node = new BlendNode(creationId++, name);
		node->size = { 5,5 };
		node->headerColor = OPERATOR_NODE_HEADER_COLOR;
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "  O  ", ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin input2 = Pin(creationId++, "  O  ", ed::PinKind::Input);
		input2.node = node;
		node->inputs.emplace_back(input2);

		Pin output = Pin(creationId++, "  O  ",ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return node;
	}

	Node* PanelNodes::CreateClampNode(const char* name)
	{
		ClampNode* node = new ClampNode(creationId++, name);
		node->size = { 5,5 };
		node->headerColor = OPERATOR_NODE_HEADER_COLOR;
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "  O  ", ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin output = Pin(creationId++, "  O  ", ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return node;
	}

	Node* PanelNodes::CreateMaxMinNode(const char* name, bool isMax)
	{
		MaxMinNode* node = new MaxMinNode(creationId++, name, isMax);
		node->size = { 5,5 };
		node->headerColor = OPERATOR_NODE_HEADER_COLOR;
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "  O  ", ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin input2 = Pin(creationId++, "  O  ", ed::PinKind::Input);
		input2.node = node;
		node->inputs.emplace_back(input2);

		Pin output = Pin(creationId++, "  O  ", ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return node;
	}

	Node* PanelNodes::CreateSingleInstructionNode(const char* name, SingleInstructionType instructionType)
	{
		SingleInstructionNode* node = new SingleInstructionNode(creationId++, name, instructionType);
		node->size = { 5,5 };
		node->headerColor = OPERATOR_NODE_HEADER_COLOR;
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "  O  ", ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin output = Pin(creationId++, "  O  ", ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		return node;
	}

	Node* PanelNodes::CreateGroupNode(const char* name)
	{
		GroupNode* node = new GroupNode(creationId++ , name);
		node->type = NodeType::GROUP;

		ImVec2 padding = { 16, 35 };
		node->pos = lastSelectionBounds.GetTL() - padding;
		ed::SetNodePosition(node->id, node->pos);

		node->size = lastSelectionBounds.GetSize() + padding;
		nodes.emplace_back(node);

		lastSelectionBounds = { 0,0,0,0 };
		ed::ClearSelection();
		return node;
	}

	void PanelNodes::UpdateNode(Pin* inputPin, Pin* outputPin, const LinkInfo& link, bool resetData)
	{
		assert(inputPin && outputPin && "Pins are null");
		assert(inputPin->node && outputPin->node && "Nodes are null");

		const auto inNode = (Node*)inputPin->node;

		switch (outputPin->node->type)
		{
			case NodeType::TRANSFORM:
			{
				UpdateNodeWithSingleInputTexture<TransformNode>(outputPin->node, inNode->texture.get(), resetData);
				break;
			}
			case NodeType::NORMAL_MAP:
			{
				UpdateNodeWithSingleInputTexture<NormalMapNode>(outputPin->node, inNode->texture.get(), resetData);
				break;
			}
			case NodeType::GRADIENT:
			{
				UpdateNodeWithSingleInputTexture<GradientNode>(outputPin->node, inNode->texture.get(), resetData);
				break;
			}
			case NodeType::EDGE_DETECTOR:
			{
				UpdateNodeWithSingleInputTexture<EdgeDetectorNode>(outputPin->node, inNode->texture.get(), resetData);
				break;
			}
			case NodeType::TWIRL:
			{
				UpdateNodeWithSingleInputTexture<TwirlNode>(outputPin->node, inNode->texture.get(), resetData);
				break;
			}
			case NodeType::INVERT:
			{
				UpdateNodeWithSingleInputTexture<InvertNode>(outputPin->node, inNode->texture.get(), resetData);
				break;
			}
			case NodeType::BLEND:
			{
				const auto n = (BlendNode*)outputPin->node;

				Texture2D* tex = nullptr;
				resetData ? tex = Node::GetWhite() : tex = inNode->texture.get();

				n->inputs[0].id.Get() < outputPin->id.Get() ? n->SetSecondTexture(tex) : n->SetFirstTexture(tex);

				break;
			}
			case NodeType::CLAMP:
			{
				UpdateNodeWithSingleInputTexture<ClampNode>(outputPin->node, inNode->texture.get(), resetData);
				break;
			}
			case NodeType::MAX:
			{
				const auto n = (MaxMinNode*)outputPin->node;

				Texture2D* tex = nullptr;
				resetData ? tex = Node::GetWhite() : tex = inNode->texture.get();

				n->inputs[0].id.Get() < outputPin->id.Get() ? n->SetSecondTexture(tex) : n->SetFirstTexture(tex);

				break;
			}
			case NodeType::MIN:
			{
				const auto n = (MaxMinNode*)outputPin->node;

				Texture2D* tex = nullptr;
				resetData ? tex = Node::GetWhite() : tex = inNode->texture.get();

				n->inputs[0].id.Get() < outputPin->id.Get() ? n->SetSecondTexture(tex) : n->SetFirstTexture(tex);

				break;
			}
			case NodeType::POW:
			case NodeType::ADD:
			case NodeType::SUBSTRACT:
			{
				UpdateNodeWithSingleInputTexture<SingleInstructionNode>(outputPin->node, inNode->texture.get(), resetData);
				break;
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

	void PanelNodes::CreateFinalOutputNode()
	{
		Node* node = new Node(OUTPUT_NODE_ID, "PBR");
		node->pos = { 1100, 100 };

		Pin albedo = Pin(OUTPUT_ALBEDO_PIN_ID, "Albedo", ed::PinKind::Input);
		albedo.node = node;
		node->inputs.emplace_back(albedo);

		Pin normals = Pin(OUTPUT_NORMALS_PIN_ID, "Normals", ed::PinKind::Input);
		normals.node = node;
		node->inputs.emplace_back(normals);

		Pin metallic = Pin(OUTPUT_METALLIC_PIN_ID, "Metallic", ed::PinKind::Input);
		metallic.node = node;
		node->inputs.emplace_back(metallic);

		Pin roughness = Pin(OUTPUT_ROUGHNESS_PIN_ID, "Roughness", ed::PinKind::Input);
		roughness.node = node;
		node->inputs.emplace_back(roughness);

		node->headerColor = ImColor(100, 100, 100, 255);

		nodes.emplace_back(node);

		creationId += 5;
		repositionNodes = true;
	}

	void PanelNodes::SaveNodes(SerializerObject& appObject)
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject panelNodesObject = JSONSerializer::GetObjectWithValue(value);

		ImVec2 pos = ed::GetNodePosition(nodes[0]->id);
		JSONSerializer::SetVector2f(appObject, "pos", glm::vec2(pos.x, pos.y));

		JSONSerializer::SetNumber(appObject, "creationId", creationId);

		SerializerValue nodesArrayValue = JSONSerializer::CreateArrayValue();
		SerializerArray nodesArray = JSONSerializer::CreateArrayFromValue(nodesArrayValue);
		JSONSerializer::SetObjectValue(appObject, "nodes", nodesArrayValue);

		for (int i = 1; i < nodes.size(); ++i)
		{
			Node* node = nodes[i];
			SerializerValue nodeValue = node->Save();
			SerializerObject nodeObject = JSONSerializer::GetObjectWithValue(nodeValue);
			pos = ed::GetNodePosition(node->id);
			JSONSerializer::SetVector2f(nodeObject, "pos", glm::vec2(pos.x, pos.y));
			if (node->type == NodeType::GROUP)
			{
				JSONSerializer::SetVector2f(nodeObject, "size", glm::vec2(node->size.x, node->size.y));
			}

			JSONSerializer::AppendValueToArray(nodesArray, nodeValue);

			SerializerValue inputPinsArrayValue = JSONSerializer::CreateArrayValue();
			SerializerArray inputPinsArray = JSONSerializer::CreateArrayFromValue(inputPinsArrayValue);
			JSONSerializer::SetObjectValue(nodeObject, "inputPins", inputPinsArrayValue);
			for (const auto& pin : node->inputs)
			{
				JSONSerializer::AppendNumberToArray(inputPinsArray, pin.id.Get());
			}

			SerializerValue outputPinsArrayValue = JSONSerializer::CreateArrayValue();
			SerializerArray outputPinsArray = JSONSerializer::CreateArrayFromValue(outputPinsArrayValue);
			JSONSerializer::SetObjectValue(nodeObject, "outputPins", outputPinsArrayValue);
			for (const auto& pin : node->outputs)
			{
				JSONSerializer::AppendNumberToArray(outputPinsArray, pin.id.Get());
			}

		}

		SerializerValue linksArrayValue = JSONSerializer::CreateArrayValue();
		SerializerArray linksArray = JSONSerializer::CreateArrayFromValue(linksArrayValue);
		JSONSerializer::SetNumber(appObject, "linkCreationId", linkCreationId);
		JSONSerializer::SetObjectValue(appObject, "links", linksArrayValue);

		for (int i = 0; i < links.size(); ++i)
		{
			LinkInfo& link = links[i];
			SerializerValue value = JSONSerializer::CreateValue();
			SerializerObject object = JSONSerializer::CreateObjectFromValue(value);
			
			JSONSerializer::SetNumber(object, "id", link.id.Get());
			JSONSerializer::SetNumber(object, "inputPinId", link.inputId.Get());
			JSONSerializer::SetNumber(object, "outputPinId", link.outputId.Get());
			JSONSerializer::AppendValueToArray(linksArray, value);
		}
	}

	void PanelNodes::LoadNodes(SerializerObject& appObject)
	{
		ClearNodes();
		
		// Load the new nodes
		repositionNodes = true;
	
		glm::vec2 pos = JSONSerializer::GetVector2fFromObject(appObject, "pos");
		nodes[0]->pos = ImVec2(pos.x, pos.y);

		SerializerArray nodesArray = JSONSerializer::GetArrayFromObject(appObject, "nodes");
		size_t size = JSONSerializer::GetArraySize(nodesArray);

		for (size_t i = 0; i < size; ++i)
		{
			SerializerObject object = JSONSerializer::GetObjectFromArray(nodesArray, i);

			const char* name = JSONSerializer::GetStringFromObject(object, "name");
			int id = JSONSerializer::GetNumberFromObject(object, "id");
			NodeType type = (NodeType)JSONSerializer::GetNumberFromObject(object, "type");

			Node* node = nullptr;
			switch (type)
			{
				case NodeType::COLOR:
				{
					node = CreateFlatColorNode(name, {});
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::GRADIENT:
				{
					node = CreateFilterNode<GradientNode>("Gradient");
					node->id = id;
					node->Load(object);
					break;
				}

				// Generators
				case NodeType::CIRCLE:
				{
					node = CreateGeneratorNode<CircleNode>(name);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::CHECKERS:
				{
					node = CreateGeneratorNode<CheckersNode>(name);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::WHITE_NOISE:
				{
					node = CreateGeneratorNode<NoiseNode>(name, NoiseType::WHITE);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::FBM_NOISE:
				{
					node = CreateGeneratorNode<NoiseNode>(name, NoiseType::FBM);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::DERIVATIVE_NOISE:
				{
					node = CreateGeneratorNode<NoiseNode>(name, NoiseType::DERIVATIVE);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::GRADIENT_NOISE:
				{
					node = CreateGeneratorNode<NoiseNode>(name, NoiseType::GRADIENT);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::VORONOI:
				{
					node = CreateGeneratorNode<VoronoiNode>(name);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::KIFS_FRACTAL:
				{
					node = CreateGeneratorNode<KifsFractalNode>(name);
					node->id = id;
					node->Load(object);
					break;
				}

				// Filters
				case NodeType::NORMAL_MAP:
				{
					node = CreateFilterNode<NormalMapNode>(name);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::EDGE_DETECTOR:
				{
					node = CreateFilterNode<EdgeDetectorNode>(name);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::TWIRL:
				{
					node = CreateFilterNode<TwirlNode>(name);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::INVERT:
				{
					node = CreateFilterNode<InvertNode>(name);
					node->id = id;
					node->Load(object);
					break;
				}

				// Operators
				case NodeType::BLEND:
				{
					node = (BlendNode*)CreateBlendNode(name);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::CLAMP:
				{
					node = (ClampNode*)CreateClampNode(name);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::MAX:
				{
					node = (MaxMinNode*)CreateMaxMinNode(name, true);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::MIN:
				{
					node = (MaxMinNode*)CreateMaxMinNode(name, false);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::POW:
				{
					node = (SingleInstructionNode*)CreateSingleInstructionNode(name, SingleInstructionType::POW);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::ADD:
				{
					node = (SingleInstructionNode*)CreateSingleInstructionNode(name, SingleInstructionType::ADD);
					node->id = id;
					node->Load(object);
					break;
				}
				case NodeType::SUBSTRACT:
				{
					node = (SingleInstructionNode*)CreateSingleInstructionNode(name, SingleInstructionType::SUBTRACT);
					node->id = id;
					node->Load(object);
					break;
				}

				// Transform
				case NodeType::TRANSFORM:
				{
					node = CreateFilterNode<TransformNode>(name);
					node->id = id;
					node->Load(object);
					break;
				}

				// Group
				case NodeType::GROUP:
				{
					node = CreateGroupNode(name);
					node->id = id;
					node->Load(object);
					break;
				}

			}

			if (node)
			{
				// Load Pins -----------------------------------------------------------------------------------
				// Assign each pin its corresponding ID so later everything works
				SerializerArray inputPinsArray = JSONSerializer::GetArrayFromObject(object, "inputPins");
				size_t inputPinsSize = JSONSerializer::GetArraySize(inputPinsArray);
				for (int i = 0; i < inputPinsSize; ++i)
				{
					node->inputs[i].id = JSONSerializer::GetNumberFromArray(inputPinsArray, i);
				}

				SerializerArray outputPinsArray = JSONSerializer::GetArrayFromObject(object, "outputPins");
				size_t outputPinsSize = JSONSerializer::GetArraySize(outputPinsArray);
				for (int i = 0; i < outputPinsSize; ++i)
				{
					node->outputs[i].id = JSONSerializer::GetNumberFromArray(outputPinsArray, i);;
				}
				// Load Pins -----------------------------------------------------------------------------------


				// Store node position and size if it is a group node
				glm::vec2 position = JSONSerializer::GetVector2fFromObject(object, "pos");
				node->pos = ImVec2(position.x, position.y);

				if (node->type == NodeType::GROUP)
				{
					glm::vec2 size = JSONSerializer::GetVector2fFromObject(object, "size");
					node->size = ImVec2(size.x, size.y);
				}

				SerializerArray nextNodesIdsArray = JSONSerializer::GetArrayFromObject(object, "nextNodesIds");
				size_t nextNodesIdsSize = JSONSerializer::GetArraySize(outputPinsArray);
				node->nextNodesIds.resize(nextNodesIdsSize);
				for (int i = 0; i < nextNodesIdsSize; i++)
				{
					node->nextNodesIds[i] = JSONSerializer::GetNumberFromArray(nextNodesIdsArray, i);
				}


			}
		}
		creationId = JSONSerializer::GetNumberFromObject(appObject, "creationId");


		// Load Links --------------------------------------------------------------------------------
		SerializerArray linksArray = JSONSerializer::GetArrayFromObject(appObject, "links");
		size = JSONSerializer::GetArraySize(linksArray);
		
		for (size_t i = 0; i < size; ++i)
		{
			SerializerObject object = JSONSerializer::GetObjectFromArray(linksArray, i);
			int id = JSONSerializer::GetNumberFromObject(object, "id");
			int inputPinId = JSONSerializer::GetNumberFromObject(object, "inputPinId");
			int outputPinId = JSONSerializer::GetNumberFromObject(object, "outputPinId");

			LinkInfo link = LinkInfo(id, inputPinId, outputPinId);
			links.push_back(link);
			auto inPin = NodeHelpers::FindPin(inputPinId, nodes);
			auto outPin = NodeHelpers::FindPin(outputPinId, nodes);
			UpdateNode(inPin, outPin, link, false);
		}
		// Load Links --------------------------------------------------------------------------------

		linkCreationId = JSONSerializer::GetNumberFromObject(appObject, "linkCreationId");

	}

	void PanelNodes::ClearNodes()
	{
		for (int i = 0; i < nodes.size(); ++i)
		{
			delete nodes[i];
		}
		nodes.clear();

		links.clear();

		CreateFinalOutputNode();
	}

	Node* PanelNodes::GetSelectedNode()
	{
		ed::NodeId selectedId;
		ed::GetSelectedNodes(&selectedId, 1);
		return NodeHelpers::FindNode(selectedId, nodes);
	}

}