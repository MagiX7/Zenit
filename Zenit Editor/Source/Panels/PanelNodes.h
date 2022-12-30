#pragma once

#include "Nodes/Node.h"

#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>

#include <vector>

namespace ed = ax::NodeEditor;

namespace Zenit {

	class EditorLayer;
	class PanelInspector;
	enum class NoiseType;

	class PanelNodes
	{
	public:
		PanelNodes() {};
		PanelNodes(EditorLayer* edLayer);
		virtual ~PanelNodes();

		void Update(TimeStep ts);
		void OnImGuiRender(PanelInspector* panelInspector);

	private:
		void DrawNodes(std::vector<Node*>& nodes, std::vector<LinkInfo>& links);
		void DrawGroupNode(Node* node);
		void HandleNodes(std::vector<Node*>& nodes);
		void HandleLinks(std::vector<LinkInfo>& links);
		void ShowNodeCreationPopup();

		Node* FindNode(ed::NodeId id) const;
		Pin* FindPin(ed::PinId id);
		LinkInfo* FindLink(const ed::LinkId& id);

		void DeleteNode(ed::NodeId id);
		void DeleteLink(const ed::LinkId& id);

		Node* CreateFlatColorNode(const char* name, const glm::vec3& color);

		// Generators
		Node* CreateNoiseNode(const char* name, NoiseType noiseType);
		Node* CreateVoronoiNode(const char* name);
		Node* CreateVector1Node(const char* name);
		Node* CreateCircleNode(const char* name);
		// Generators
		
		// Filters
		Node* CreateNormalMapNode(const char* name);
		Node* CreateTwirlNode(const char* name);
		// Filters
		
		// Operators
		Node* CreateBlendNode(const char* name);
		Node* CreateClampNode(const char* name);
		Node* CreateMinNode(const char* name);
		Node* CreateMaxNode(const char* name);
		// Operators

		// Helpers
		void CreateGroupNode(const char* name);
		// Helpers

		void UpdateNode(Pin* startPin, Pin* endPin, bool resetData);
		void UpdateOutputNodeData(Pin& startPin, Pin& endPin, bool resetData);

	private:
		EditorLayer* editorLayer;

		std::vector<Node*> nodes;
		std::vector<LinkInfo> links;
		int creationId = 1;
		int linkCreationId = 1000;

		ed::Detail::EditorContext* context;
		ed::Config config;
		Pin incorrectPin = Pin(-1, "", PinType::None, ed::PinKind::None);
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