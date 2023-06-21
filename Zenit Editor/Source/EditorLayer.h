#pragma once

#include "Zenit.h"
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
		void ExportTextures(int resolution, int channels);
		void Save(bool forceDialog);
		bool Load();
		void NewScene();

		void LoadSkyboxes();
		void ReloadSkyboxes();

		void LoadModels();
		void ReloadModels();

	private:
		bool finished = false;
		PanelViewport panelViewport;
		PanelLayerStack panelLayerStack;
		PanelNodes* panelNodes;

		std::unique_ptr<FrameBuffer> fbo;

		Skybox* currentSkybox;
		std::vector<Skybox*> skyboxes;
		std::vector<std::future<void>> futures;
		bool reloadSkyboxes = false;

		PerspectiveCamera camera;
		Frustum frustum;
		std::vector<Model*> models;
		Model* currentModel;
		Mesh* currentMesh;

		std::unique_ptr<Shader> pbrShader;
		Texture2D* diffuse;
		Texture2D* normals;
		Texture2D* metallic;
		Texture2D* roughness;

		DirectionalLight dirLight;
		SkyboxProperties skyboxProps;

		// Final textures
		Node* diffuseOutput = nullptr;
		Node* normalsOutput = nullptr;
		Node* metallicOutput = nullptr;
		Node* roughnessOutput = nullptr;
		Node* aoOutput = nullptr;
		bool showExportingPanel = false;

		std::stack<Layer*> layers;

		SerializerValue serializerRootValue;
		std::string savedFilePath = "";


		friend class PanelNodes;
	};

}
