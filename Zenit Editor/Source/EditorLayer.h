#pragma once

#include "Zenit.h"
#include "Panels/PanelInspector.h"
#include "Panels/PanelViewport.h"
#include "Panels/PanelSkybox.h"

#include "Helpers/SkyboxProperties.h"

#include <imgui_node_editor.h>
#include "Helpers/Node.h"

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

	private:
		void DrawSkybox();
		void SetModelShaderData();
		void ExportTextures();
		Node* CreateFlatColorNode(const char* name, glm::vec4 color);
		void HandleNodes();
		void DrawNodes();

	private:
		PanelInspector panelInspector;
		PanelViewport panelViewport;
		PanelSkybox panelSkybox;

		std::unique_ptr<FrameBuffer> fbo;
		std::unique_ptr<FrameBuffer> diffuseFbo;

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
		std::vector<Node*> nodes;
		bool showCreationPopup = false;
		
	};
}