#pragma once

#include "Zenit.h"
#include "Panels/PanelInspector.h"
#include "Panels/PanelViewport.h"
#include "Panels/PanelSkybox.h"
#include "Panels/PanelLayerStack.h"
#include "Panels/PanelNodes.h"

#include "Helpers/SkyboxProperties.h"
#include "Nodes/Node.h"

#include <imgui_node_editor.h>

#include <memory>
#include <stack>

namespace Zenit {

	struct DirectionalLight
	{
		glm::vec3 dir = { 0,0,0 };
		glm::vec3 ambient = { 1,1,1 };
		glm::vec3 diffuse = { 1,1,1 };
	};

	class EditorLayer : public EngineLayer
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
		
		bool SetDiffuseData();

	private:
		PanelInspector panelInspector;
		PanelViewport panelViewport;
		PanelSkybox panelSkybox;
		PanelLayerStack panelLayerStack;
		PanelNodes* panelNodes;

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

		// Final textures
		Node* diffuseOutput = nullptr;
		Node* normalsOutput = nullptr;
		Node* metallicOutput = nullptr;
		Node* roughnessOutput = nullptr;
		Node* aoOutput = nullptr;

		std::stack<Layer*> layers;


		friend class PanelNodes;
	};

}
