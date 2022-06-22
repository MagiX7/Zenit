#include "EditorLayer.h"
#include "Zenit/Core/Log.h"

#include <ImGui/imgui.h>
#include <stb_image/stb_image_write.h>

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
		fbo = std::make_unique<FrameBuffer>(2048, 2048, 1);

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
		normal = std::make_unique<Texture2D>(&data, 1, 1);
		metallic = std::make_unique<Texture2D>(&data, 1, 1);
		roughness = std::make_unique<Texture2D>(&data, 1, 1);
		ambientOcclusion = std::make_unique<Texture2D>(&data, 1, 1);

		dirLight = DirectionalLight();
		skyboxProps = SkyboxProperties();

		context = ed::CreateEditor();
	}

	void EditorLayer::OnDetach()
	{
		ed::DestroyEditor(context);
	}

	void EditorLayer::OnUpdate(const TimeStep ts)
	{
		camera.Update(ts);
		model->Update(ts);

		fbo->Bind();
		{
			Renderer3D::Clear({ 0.15,0.15,0.15,1 });

			DrawSkybox();
			
			pbrShader->Bind();
			SetModelShaderData();

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
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Export"))
			{
				ExportTextures();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Show Demo Window"))
				showDemoWindow = !showDemoWindow;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();


		panelViewport.OnImGuiRender(fbo.get(), camera);
		panelInspector.OnImGuiRender(model, dirLight);
		panelSkybox.OnImGuiRender(skybox, skyboxProps);

		HandleNodes();

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

	void EditorLayer::SetModelShaderData()
	{
		pbrShader->SetUniformMatrix4f("view", camera.GetView());
		pbrShader->SetUniformMatrix4f("projection", camera.GetProjection());

		pbrShader->SetUniformMatrix4f("model", model->GetTransform());
		pbrShader->SetUniformVec3f("camPos", camera.GetPosition());

		pbrShader->SetUniformVec3f("dirLight.direction", dirLight.dir);
		pbrShader->SetUniformVec3f("dirLight.ambient", dirLight.ambient);
		pbrShader->SetUniformVec3f("dirLight.diffuse", dirLight.diffuse);

		diffuse->Bind();
		pbrShader->SetUniform1i("diffuseTexture", 0);

		normal->Bind(1);
		pbrShader->SetUniform1i("normalsTexture", 1);

		metallic->Bind(2);
		pbrShader->SetUniform1i("metallicTexture", 2);

		roughness->Bind(3);
		pbrShader->SetUniform1i("roughnessTexture", 3);

		ambientOcclusion->Bind(4);
		pbrShader->SetUniform1i("ambientOcclusionTexture", 4);


		pbrShader->SetUniform1i("drawSkybox", skyboxProps.draw);
		if (skyboxProps.draw)
		{
			skybox->Bind(5);
			pbrShader->SetUniform1i("skybox", 5);

			pbrShader->SetUniform1f("skyboxIntensity", skybox->GetInstensity());
			pbrShader->SetUniform1i("skyboxReflectionEnabled", skybox->IsReflectionEnabled());

		}
	}

	void EditorLayer::ExportTextures()
	{
		constexpr int channels = 3;
		const int w = fbo->GetWidth();
		const int h = fbo->GetHeight();
		GLubyte* data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);
		
		fbo->Bind();
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png("mytex.png", w, h, channels, data, w * channels);
		delete[] data;
	}

	void EditorLayer::HandleNodes() const
	{
		ed::SetCurrentEditor(context);

		ImGui::Begin("Node editor");
		ed::Begin("");
		int uniqueId = 1;

		ed::BeginNode(uniqueId++);
		ImGui::Text("Node A");
		ed::BeginPin(uniqueId++, ed::PinKind::Input);
		ImGui::Text("-> In");
		ed::EndPin();
		ImGui::SameLine();
		ed::BeginPin(uniqueId++, ed::PinKind::Output);
		ImGui::Text("Out ->");
		ed::EndPin();
		ed::EndNode();
		ed::End();
		ImGui::End();
	}
}
