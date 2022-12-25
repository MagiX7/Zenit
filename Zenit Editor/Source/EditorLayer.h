#pragma once

#include "Zenit.h"
#include "Panels/PanelInspector.h"
#include "Panels/PanelViewport.h"
#include "Panels/PanelLayerStack.h"
#include "Panels/PanelNodes.h"

#include "Helpers/SkyboxProperties.h"
#include "Nodes/Node.h"

#include <imgui_node_editor.h>

#include <memory>
#include <stack>
#include <future>
#include <mutex>

namespace Zenit {

	struct DirectionalLight
	{
		glm::vec3 dir = { 1, 1, 1 };
		glm::vec3 color = { 1,1,1 };
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
		void SetModelShaderData();
		void ExportTextures();

		void LoadSkyboxes();
		void ReloadSkyboxes();

		void LoadModels();
		void ReloadModels();

	private:
		bool finished = false;
		PanelInspector panelInspector;
		PanelViewport panelViewport;
		PanelLayerStack panelLayerStack;
		PanelNodes* panelNodes;

		std::unique_ptr<FrameBuffer> fbo;

		Skybox* currentSkybox;
		std::vector<Skybox*> skyboxes;
		std::vector<std::future<void>> futures;
		bool reloadSkyboxes = false;

		PerspectiveCamera camera;
		std::vector<Model*> models;
		Model* currentModel;

		std::unique_ptr<Shader> pbrShader;
		Texture2D* diffuse;
		Texture2D* normals;
		Texture2D* metallic;
		std::shared_ptr<Texture2D> white;
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
