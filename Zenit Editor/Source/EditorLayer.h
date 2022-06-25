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
		glm::vec3 ambient = { 1,0,0 };
		glm::vec3 diffuse = { 0,0,1 };
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

		// Nodes =============
	private:
		Node* CreateFlatColorNode(const char* name, const glm::vec3& color);
		Node* CreatePerlinNoiseNode(const char* name);



		void HandleNodes();
		void DrawNodes();
		Node* FindNode(ed::NodeId id);
		
		// Nodes =============

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
		std::unique_ptr<Texture2D> diffuse;
		std::unique_ptr<Texture2D> normal;
		std::unique_ptr<Texture2D> metallic;
		std::unique_ptr<Texture2D> roughness;
		std::unique_ptr<Texture2D> ambientOcclusion;

		DirectionalLight dirLight;
		SkyboxProperties skyboxProps;

		// Nodes-related
		ed::EditorContext* context;
		ed::Config config;
		std::vector<Node*> nodes;
		std::vector<LinkInfo> links;
		bool showCreationPopup = false;
		int creationId = 1;
		
	};
}