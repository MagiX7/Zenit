#include "EditorLayer.h"

#include <ImGui/imgui.h>

namespace Zenit {

	EditorLayer::EditorLayer() : camera(PerspectiveCamera({ -5,5,-5 }, { 0,0,0 }, 45.0f))
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		fbo = std::make_unique<FrameBuffer>(1280, 720, 0);

		gun = ModelImporter::ImportModel("Assets/Models/Gun/Gun.dae");
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		camera.Update(ts);

		Renderer3D::Clear({ 0.2,0.2,0.2,1 });
		fbo->Bind();
		gun->Draw(camera);
		fbo->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool showDemoWindow = false;
		ImGui::BeginMainMenuBar();
		if (ImGui::MenuItem("File"))
		{

		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Show Demo Window"))
				showDemoWindow = !showDemoWindow;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		ImGui::Begin("Viewport");
		ImGui::Image((void*)fbo->GetColorId(), { (float)fbo->GetWidth(), (float)fbo->GetHeight() }, { 0,1 }, { 1,0 });
		ImGui::End();

		panelInspector.OnImGuiRender();

		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);

	}
}