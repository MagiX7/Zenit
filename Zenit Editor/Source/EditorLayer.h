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
		glm::vec3 dir = { 35, 30, -8 };
		glm::vec3 ambient = { 0,0,0 };
		glm::vec3 diffuse = { 1,1,1 };
		glm::vec3 specular = { 1,1,1 };
		float intensity = 1.0f;
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

		bool SetDiffuseData(Node* node);
		bool SetNormalsData(Node* node);
		bool SetMetallicData(Node* node);
		bool SetRoughnessData(Node* node);

	private:
		void DrawSkybox();
		void SetModelShaderData();
		void ExportTextures();		

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
		Texture2D* white;
		float metallicValue = 1.0f;
		Texture2D* roughness;
		float roughnessValue = 1.0f;
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
