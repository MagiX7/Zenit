#pragma once

#include "Nodes/Node.h"

#include <imgui_node_editor.h>

#include <vector>

namespace ed = ax::NodeEditor;

namespace Zenit {

	class EditorLayer;
	class PanelInspector;

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
		void HandleLinks(std::vector<LinkInfo>& links);
		void ShowNodeCreationPopup();
		void ShowNodeOptionsPopup();

		Node* FindNode(ed::NodeId id) const;
		Pin* FindPin(ed::PinId id) const;
		LinkInfo* FindLink(const ed::LinkId& id) const;

		void DeleteNode(ed::NodeId id);
		void DeleteLink(const ed::LinkId& id) const;

		Node* CreateFlatColorNode(const char* name, const glm::vec3& color);
		Node* CreateNoiseNode(const char* name);
		Node* CreateVoronoiNode(const char* name);
		Node* CreateVector1Node(const char* name);

		void UpdateOutputNodeData(Pin& startPin, Pin& endPin, bool resetData);

	private:
		EditorLayer* editorLayer;

		std::vector<Node*> nodes;
		std::vector<LinkInfo> links;
		int creationId = 1;

		ed::EditorContext* context;
		ed::Config config;

		ed::NodeId rightClickedNodeId = 0;
		ed::NodeId selectedNodeId = 0;

		bool showCreationPopup = false;
		bool showNodePopup = false;

		Node* diffuseNode = nullptr;
		Node* normalsNode = nullptr;
		Node* metallicNode = nullptr;
		Node* roughnessNode = nullptr;


	};

}