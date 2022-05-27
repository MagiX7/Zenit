#include "EditorLayer.h"
#include "Zenit/Core/Log.h"

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

		std::vector<std::string> faces;
		
		faces.push_back("Assets/Skyboxes/Sea/right.jpg");
		faces.push_back("Assets/Skyboxes/Sea/left.jpg");
		faces.push_back("Assets/Skyboxes/Sea/top.jpg");
		faces.push_back("Assets/Skyboxes/Sea/bottom.jpg");
		faces.push_back("Assets/Skyboxes/Sea/front.jpg");
		faces.push_back("Assets/Skyboxes/Sea/back.jpg");
		
		skybox = std::make_unique<Skybox>(faces);

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
		glDisable(GL_CULL_FACE);
		skybox->Draw(camera);
		glEnable(GL_CULL_FACE);
		model->Draw(camera, skybox);

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
		panelSkybox.OnImGuiRender(skybox);

		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}
}