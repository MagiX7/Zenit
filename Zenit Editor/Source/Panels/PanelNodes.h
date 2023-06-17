#pragma once

#include "Nodes/Node.h"

#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>

#include <vector>

namespace ed = ax::NodeEditor;

#define FLAT_COLOR_NODE_HEADER_COLOR ImColor(120, 80, 80, 255);
#define FILTER_NODE_HEADER_COLOR ImColor(80, 80, 150, 255);
#define GENERATOR_NODE_HEADER_COLOR ImColor(80, 120, 80, 255);
#define OPERATOR_NODE_HEADER_COLOR ImColor(120, 80, 150, 255);

namespace Zenit {

	class EditorLayer;
	enum class NoiseType;
	enum class SingleInstructionType;

	class PanelNodes
	{
	public:
		PanelNodes() {};
		PanelNodes(EditorLayer* edLayer);
		virtual ~PanelNodes();

		void Update(TimeStep ts);
		void OnImGuiRender();

		//Node* FindNode(ed::NodeId id) const;
		//Pin* FindPin(ed::PinId id);
		//LinkInfo* FindLink(const ed::LinkId& id);

		void SaveNodes(SerializerObject& appObject);
		void LoadNodes(SerializerObject& appObject);

		static const std::vector<Node*>& GetNodes() { return nodes; }
		Node* GetSelectedNode();

	private:
		void DrawNodes(std::vector<Node*>& nodes, std::vector<LinkInfo>& links);
		void DrawGroupNode(Node* node);
		void HandleNodes(std::vector<Node*>& nodes);
		void HandleLinks(std::vector<LinkInfo>& links);
		void ShowNodeCreationPopup();

		void DeleteNode(ed::NodeId id);
		void DeleteLink(const ed::LinkId& id);


		Node* CreateFlatColorNode(const char* name, const glm::vec3& color);
		
		// Operators
		Node* CreateBlendNode(const char* name);
		Node* CreateClampNode(const char* name);
		Node* CreateMaxMinNode(const char* name, bool isMax);
		Node* CreateSingleInstructionNode(const char* name, SingleInstructionType instructionType);
		// Operators

		// Helpers
		Node* CreateGroupNode(const char* name);
		// Helpers


		template<typename T, typename... Args>
		T* CreateGeneratorNode(const char* name, Args&&... args)
		{
			T* node = new T(creationId++, name, std::forward<Args>(args)...);
			node->size = { 5,5 };
			node->headerColor = GENERATOR_NODE_HEADER_COLOR;
			nodes.emplace_back(node);

			Pin output = Pin(creationId++, "Output", ed::PinKind::Output);
			output.node = node;
			node->outputs.emplace_back(output);

			return node;
		}

		template<typename T, typename... Args>
		T* CreateFilterNode(const char* name, Args&&... args)
		{
			T* node = new T(creationId++, name, std::forward<Args>(args)...);
			node->size = { 5,5 };
			node->headerColor = FILTER_NODE_HEADER_COLOR;
			nodes.emplace_back(node);

			Pin input = Pin(creationId++, "Input", ed::PinKind::Input);
			input.node = node;
			node->inputs.emplace_back(input);

			Pin output = Pin(creationId++, "Output", ed::PinKind::Output);
			output.node = node;
			node->outputs.emplace_back(output);

			return node;
		}

		void UpdateNode(Pin* startPin, Pin* endPin, const LinkInfo& link, bool resetData);
		void UpdateOutputNodeData(Pin& startPin, Pin& endPin, bool resetData);

		template<typename T>
		void UpdateNodeWithSingleInputTexture(Node* affectedNode, Texture2D* tex, bool resetData)
		{
			T* n = (T*)affectedNode;
			resetData ? n->SetInputTexture(Node::GetWhite()) : n->SetInputTexture(tex);
		}

		void CreateFinalOutputNode();
		
	private:
		EditorLayer* editorLayer;

		std::shared_ptr<Texture2D> nodeBgTexture;
		static std::vector<Node*> nodes;
		std::vector<LinkInfo> links;
		int creationId = 1;
		int linkCreationId = 1000;
		bool repositionNodes = false;

		ed::Detail::EditorContext* context;
		ed::Config config;
		Pin incorrectPin = Pin(-1, "", ed::PinKind::None);
		LinkInfo incorrectLink = LinkInfo(-1, -1, -1);

		ed::NodeId rightClickedNodeId = 0;

		bool showCreationPopup = false;
		bool showGroupPopup = false;

		Node* diffuseNode = nullptr;
		Node* normalsNode = nullptr;
		Node* metallicNode = nullptr;
		Node* roughnessNode = nullptr;

		ImRect lastSelectionBounds = { 0,0,0,0 };
		bool hovered = false;
		ImVec2 panelSize;

	};

}