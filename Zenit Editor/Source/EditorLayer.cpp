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
		skyboxShader = std::make_unique<Shader>("Assets/Shaders/skybox.shader");

		uint32_t data = 0xffffffff;
		diffuse = std::make_unique<Texture2D>(&data, 1, 1);

		skyboxProps = SkyboxProperties();
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

			DrawSkybox();
			
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
		panelSkybox.OnImGuiRender(skybox, skyboxProps);

		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}

	void EditorLayer::DrawSkybox()
	{
		if (skyboxProps.draw)
		{
			glDisable(GL_CULL_FACE);
			glDepthFunc(GL_LEQUAL);

			skyboxShader->Bind();
			skyboxShader->SetUniformMatrix4f("view", glm::mat3(camera.GetView()));
			skyboxShader->SetUniformMatrix4f("projection", camera.GetProjection());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetId());
			skyboxShader->SetUniform1i("skybox", 0);

			skybox->Draw();

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			skyboxShader->Unbind();

			glDepthFunc(GL_LESS);
			glEnable(GL_CULL_FACE);
		}

	}

	void EditorLayer::SetShaderData()
	{
		pbrShader->SetUniformMatrix4f("view", camera.GetView());
		pbrShader->SetUniformMatrix4f("projection", camera.GetProjection());

		pbrShader->SetUniformMatrix4f("model", model->GetTransform());
		pbrShader->SetUniformVec3f("camPos", camera.GetPosition());

		diffuse->Bind();
		pbrShader->SetUniform1i("colorTexture", 0);

		pbrShader->SetUniform1i("drawSkybox", skyboxProps.draw);
		if (skyboxProps.draw)
		{
			skybox->Bind(1);
			pbrShader->SetUniform1i("skybox", 1);

			pbrShader->SetUniform1f("skyboxIntensity", skybox->GetInstensity());
			pbrShader->SetUniform1i("skyboxReflectionEnabled", skybox->IsReflectionEnabled());

		}
	}
}
