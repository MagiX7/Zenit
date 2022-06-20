#pragma once

#include "Zenit.h"
#include "Panels/PanelInspector.h"
#include "Panels/PanelViewport.h"
#include "Panels/PanelSkybox.h"

#include "Helpers/SkyboxProperties.h"

#include <memory>

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
	};
}