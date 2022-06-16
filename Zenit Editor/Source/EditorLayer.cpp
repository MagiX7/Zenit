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

		pbrShader = std::make_unique<Shader>("Assets/Shaders/PBR.shader");
		uint32_t data = 0xffffffff;
		diffuse = std::make_unique<Texture2D>(&data, 1, 1);

		drawSkybox = true;
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		camera.Update(ts);
		model->Update(ts);

		fbo->Bind();
		{
			Renderer3D::Clear({ 0.15,0.15,0.15,1 });

			if (drawSkybox)
			{
				glDisable(GL_CULL_FACE);
				skybox->Draw(camera);
				glEnable(GL_CULL_FACE);
			}

			pbrShader->Bind();
			SetShaderData();

			model->Draw();

			skybox->Unbind();
			pbrShader->Unbind();

		}
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


		panelViewport.OnImGuiRender(fbo.get(), camera);
		panelInspector.OnImGuiRender(model);
		panelSkybox.OnImGuiRender(skybox, drawSkybox);

		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}

	void EditorLayer::SetShaderData()
	{
		pbrShader->SetUniformMatrix4f("view", camera.GetView());
		pbrShader->SetUniformMatrix4f("projection", camera.GetProjection());

		pbrShader->SetUniformMatrix4f("model", model->GetTransform());
		pbrShader->SetUniformVec3f("camPos", camera.GetPosition());

		diffuse->Bind();
		pbrShader->SetUniform1i("colorTexture", 0);

		pbrShader->SetUniform1i("drawSkybox", drawSkybox);
		if (drawSkybox)
		{
			skybox->Bind(1);
			pbrShader->SetUniform1i("skybox", 1);

			pbrShader->SetUniform1f("skyboxIntensity", skybox->GetInstensity());
			pbrShader->SetUniform1i("skyboxReflectionEnabled", skybox->IsReflectionEnabled());
		}
	}
}
