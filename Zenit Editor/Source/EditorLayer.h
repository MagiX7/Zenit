#pragma once

#include "Zenit.h"
#include "Panels/PanelInspector.h"
#include "Panels/PanelViewport.h"
#include "Panels/PanelSkybox.h"

#include "Helpers/SkyboxProperties.h"

#include <imgui_node_editor.h>
#include "Helpers/Nodes/Node.h"

#include <memory>

namespace ed = ax::NodeEditor;

namespace Zenit {

	struct DirectionalLight
	{
		glm::vec3 dir = { 0,0,0 };
		glm::vec3 ambient = { 1,1,1 };
		glm::vec3 diffuse = { 1,1,1 };
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(const TimeStep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;

	private:
		void DrawSkybox();
		void SetModelShaderData();
		void ExportTextures();
		void ShowNodeCreationPopup();
		void ShowNodeOptionsPopup();

		// Nodes =============
	private:
		Node* CreateFlatColorNode(const char* name, const glm::vec3& color);
		Node* CreatePerlinNoiseNode(const char* name);

		template<class T>
		T* GetNode(ed::NodeId id);

		void HandleNodes();
		void HandleLinks();
		void DrawNodes();
		Node* FindNode(ed::NodeId id) const ;
		Pin* FindPin(ed::PinId id) const;
		
		// Nodes =============

		void SetDiffuseData();

	private:
		PanelInspector panelInspector;
		PanelViewport panelViewport;
		PanelSkybox panelSkybox;

		std::unique_ptr<FrameBuffer> fbo;

		std::unique_ptr<Skybox> skybox;

		PerspectiveCamera camera;
		Model* model;

		std::unique_ptr<Shader> pbrShader;
		std::unique_ptr<Shader> skyboxShader;
		Texture2D* diffuse;
		Texture2D* normal;
		Texture2D* metallic;
		Texture2D* roughness;
		Texture2D* ambientOcclusion;

		DirectionalLight dirLight;
		SkyboxProperties skyboxProps;

		// Nodes-related
		ed::EditorContext* context;
		ed::Config config;
		std::vector<Node*> nodes;
		std::vector<LinkInfo> links;
		bool showCreationPopup = false;
		bool showNodePopup = false;
		int creationId = 1;

		Node* diffuseOutput = nullptr;
		Node* normalsOutput = nullptr;
		Node* metallicOutput = nullptr;
		Node* roughnessOutput = nullptr;
		Node* aoOutput = nullptr;
		ed::NodeId rightClickedNodeId = -1;
	};

	template <class T>
	T* EditorLayer::GetNode(ed::NodeId id)
	{
		T* node = nullptr;

		for (std::vector<Node*>::iterator it = nodes.begin(); it < nodes.end(); ++it)
		{
			if ((*it)->id == id)
			{
				node = dynamic_cast<T*>(*it);
				return node;
			}
		}

		return nullptr;
	}
}
