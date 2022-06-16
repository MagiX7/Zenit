#pragma once

#include "Zenit.h"
#include "Panels/PanelInspector.h"
#include "Panels/PanelViewport.h"
#include "Panels/PanelSkybox.h"

#include <memory>

namespace Zenit {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(TimeStep ts) override;
		virtual void OnImGuiRender() override;

	private:
		void SetShaderData();

	private:
		PanelInspector panelInspector;
		PanelViewport panelViewport;
		PanelSkybox panelSkybox;
		std::unique_ptr<FrameBuffer> fbo;
		std::unique_ptr<Skybox> skybox;

		PerspectiveCamera camera;
		Model* model;

		std::unique_ptr<Shader> pbrShader;
		std::unique_ptr<Texture2D> diffuse;

		bool drawSkybox;
	};
}