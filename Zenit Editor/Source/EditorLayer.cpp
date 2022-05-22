#include "EditorLayer.h"

#include <ImGui/imgui.h>

namespace Zenit {

	EditorLayer::EditorLayer() : camera(PerspectiveCamera({ 0,0,2 }, { 0,0,0 }, 60.0f))
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		fbo = std::make_unique<FrameBuffer>(1280, 720, 0);

		//model = ModelImporter::ImportModel("Assets/Models/Cerberus/Cerberus.fbx");
		//model = ModelImporter::ImportModel("Assets/Models/Gun/Gun.dae");
		model = ModelImporter::ImportModel("Assets/Models/Cube/Cube.fbx");
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		camera.Update(ts);
		model->Update(ts);

		fbo->Bind();
		Renderer3D::Clear({ 0.05,0.05,0.05,1 });
		model->Draw(camera);
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


		panelViewport.OnImGuiRender(fbo.get(), camera, model);
		panelInspector.OnImGuiRender(model);

		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}
}