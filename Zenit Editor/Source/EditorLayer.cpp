#include "EditorLayer.h"
#include "Zenit/Core/Log.h"

#include "Nodes/ColorNode.h"
#include "Nodes/NoiseNode.h"
#include "Nodes/VoronoiNode.h"
#include "Nodes/Constants/Vec1Node.h"
#include "Helpers/Math.h"

#include <ImGui/imgui.h>
#include <stb_image/stb_image_write.h>

namespace Zenit {

	EditorLayer::EditorLayer()
		: camera(PerspectiveCamera({ 0,0,2.5 }, { 0,0,0 }, 60.0f, 1280.0f / 720.0f))
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		fbo = std::make_unique<FrameBuffer>(1280, 720, 0);

		std::vector<std::string> faces;
		faces.emplace_back("Assets/Skyboxes/Sea/right.jpg");
		faces.emplace_back("Assets/Skyboxes/Sea/left.jpg");
		faces.emplace_back("Assets/Skyboxes/Sea/top.jpg");
		faces.emplace_back("Assets/Skyboxes/Sea/bottom.jpg");
		faces.emplace_back("Assets/Skyboxes/Sea/front.jpg");
		faces.emplace_back("Assets/Skyboxes/Sea/back.jpg");
		skybox = std::make_unique<Skybox>(faces);

		model = ModelImporter::ImportModel("Assets/Models/Primitives/cube_rounded.fbx");

		pbrShader = std::make_unique<Shader>("Assets/Shaders/pbr.shader");
		skyboxShader = std::make_unique<Shader>("Assets/Shaders/skybox.shader");


		uint32_t data = 0xffffffff;
		diffuse = new Texture2D(&data, 1, 1);
		normals = new Texture2D(&data, 1, 1);
		metallic = new Texture2D(&data, 1, 1);
		roughness = new Texture2D(&data, 1, 1);
		ambientOcclusion = new Texture2D(&data, 1, 1);
		
		white = new Texture2D("Settings/white.png");

		dirLight = DirectionalLight();
		skyboxProps = SkyboxProperties();

		panelNodes = new PanelNodes(this);
	}

	void EditorLayer::OnDetach()
	{
		delete diffuse;
		delete normals;
		delete metallic;
		delete roughness;
		delete ambientOcclusion;
		delete white;

		delete panelNodes;
	}

	void EditorLayer::OnUpdate(const TimeStep ts)
	{
		camera.Update(ts);
		panelViewport.OnUpdate(ts, model, camera);
		panelNodes->Update(ts);

		fbo->Bind();
		{
			Renderer3D::Clear({ 0.05,0.05,0.05,1 });
			
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


		ImGui::Begin("Lightning Settings");
		{
			ImGui::Text("Direction");
			ImGui::SameLine();
			ImGui::DragFloat3("##Direction", glm::value_ptr(dirLight.dir));
			
			ImGui::Text("Intensity");
			ImGui::SameLine();
			ImGui::DragFloat("##Intensity", &dirLight.intensity, 0.05f, 0.0f);
			
			ImGui::Separator();

			ImGui::SetNextItemWidth(200);
			ImGui::ColorPicker3("Ambient", glm::value_ptr(dirLight.ambient));
			ImGui::SetNextItemWidth(200);
			ImGui::ColorPicker3("Diffuse", glm::value_ptr(dirLight.diffuse));
			ImGui::SetNextItemWidth(200);
			ImGui::ColorPicker3("Specular", glm::value_ptr(dirLight.specular));

		}
		ImGui::End();

		panelViewport.OnImGuiRender(fbo.get(), camera);
		panelSkybox.OnImGuiRender(skybox, skyboxProps);
		//panelLayerStack.OnImGuiRender(layers);
		panelNodes->OnImGuiRender(&panelInspector);

		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}

	void EditorLayer::OnEvent(Event& e)
	{
	}

	bool EditorLayer::SetDiffuseData(Node* node)
	{
		if (!node)
		{
			if (diffuse != white)
				diffuse = white;
			return false;
		}

		const auto n = (ComputeShaderNode*)node;
		diffuse = n->texture.get();
		return true;

		//switch (node->outputType)
		//{
		//	case NodeOutputType::TEXTURE:
		//	{
		//		const auto n = (ComputeShaderNode*)node;
		//		diffuse = n->texture.get();
		//		return true;
		//	}
		//	case NodeOutputType::FLAT_COLOR:
		//	{
		//		const auto n = (ColorNode*)node;
		//		uint32_t data = Math::GetRGBAHexadecimal(n->color);
		//		diffuse->SetData(&data);
		//		return true;
		//	}
		//}

		return false;
	}

	bool EditorLayer::SetNormalsData(Node* node)
	{
		if (!node)
		{
			if (normals != white)
				normals = white;
			return false;
		}

		const auto n = (ComputeShaderNode*)node;
		normals = n->texture.get();
		return true;

		//switch (node->outputType)
		//{
		//	case NodeOutputType::TEXTURE:
		//	{
		//		const auto n = (ComputeShaderNode*)node;
		//		normals = n->texture.get();
		//		return true;
		//	}
		//	//case NodeOutputType::FLAT_COLOR:
		//	//{
		//	//	const auto n = (ColorNode*)node;
		//	//	uint32_t data = Math::GetRGBAHexadecimal(n->color);
		//	//	normal->SetData(&data);
		//	//	return true;
		//	//}
		//}
		//
		//return false;
	}

	bool EditorLayer::SetMetallicData(Node* node)
	{
		if (!node)
			return false;

		const auto n = (ComputeShaderNode*)node;
		metallic = n->texture.get();
		return true;

		//switch (node->outputType)
		//{
		//	case NodeOutputType::TEXTURE:
		//	{
		//		const auto n = (ComputeShaderNode*)node;
		//		metallic = n->texture.get();
		//		return true;
		//	}
		//	case NodeOutputType::FLAT_COLOR:
		//	{
		//		const auto n = (ColorNode*)node;
		//		uint32_t data = Math::GetRGBAHexadecimal(n->color);
		//		metallic->SetData(&data);
		//		return true;
		//	}
		//}

		return false;
	}

	bool EditorLayer::SetRoughnessData(Node* node)
	{
		if (!node)
			return false;

		switch (node->outputType)
		{
			case NodeOutputType::TEXTURE:
			{
				const auto n = (ComputeShaderNode*)node;
				roughness = n->texture.get();
				return true;
			}
			//case NodeOutputType::FLAT_COLOR:
			//{
			//	const auto n = (ColorNode*)node;
			//	uint32_t data = Math::GetRGBAHexadecimal(n->color);
			//	roughness->SetData(&data);
			//	return true;
			//}
			case NodeOutputType::VEC1:
			{
				const auto n = (Vec1Node*)node;
				roughnessValue = n->value;
				return true;
				break;
			}
		}

		return false;
	}

	void EditorLayer::DrawSkybox()
	{
		if (!skyboxProps.draw)
			return;

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

	void EditorLayer::SetModelShaderData()
	{
		pbrShader->SetUniformMatrix4f("view", camera.GetView());
		pbrShader->SetUniformMatrix4f("projection", camera.GetProjection());

		pbrShader->SetUniformMatrix4f("model", model->GetTransform());
		pbrShader->SetUniformVec3f("camPos", camera.GetPosition());

		pbrShader->SetUniformVec3f("dirLight.direction", glm::normalize(dirLight.dir));
		pbrShader->SetUniformVec3f("dirLight.ambient", dirLight.ambient);
		pbrShader->SetUniformVec3f("dirLight.diffuse", dirLight.diffuse);
		pbrShader->SetUniformVec3f("dirLight.specular", dirLight.specular);
		pbrShader->SetUniform1f("dirLight.intensity", dirLight.intensity);

		diffuse->Bind();
		pbrShader->SetUniform1i("diffuseTexture", 0);

		normals->Bind(1);
		pbrShader->SetUniform1i("normalsTexture", 1);

		metallic->Bind(2);
		pbrShader->SetUniform1i("metallicTexture", 2);
		
		roughness->Bind(3);
		pbrShader->SetUniform1i("roughnessTexture", 3);

		ambientOcclusion->Bind(4);
		pbrShader->SetUniform1i("ambientOcclusionTexture", 4);

		//pbrShader->SetUniform1f("metallic", metallicValue);
		//pbrShader->SetUniform1f("roughness", roughnessValue);


		pbrShader->SetUniform1i("drawSkybox", skyboxProps.draw);
		if (skyboxProps.draw)
		{
			skybox->Bind(5);
			pbrShader->SetUniform1i("skybox", 5);

			pbrShader->SetUniform1f("skyboxIntensity", skybox->GetInstensity());
			//pbrShader->SetUniform1i("skyboxReflectionEnabled", skybox->IsReflectionEnabled());
			pbrShader->SetUniform1i("skyboxReflectionEnabled", 0);
		}
	}

	void EditorLayer::ExportTextures()
	{
		std::string path = FileDialog::SaveFile("png (*.png)\0*.png\0");
		if (path.empty())
			return;

		constexpr int channels = 4;
		int w = diffuse->GetWidth() == 1 ? 1024 : diffuse->GetWidth();
		int h = diffuse->GetHeight() == 1 ? 1024 : diffuse->GetHeight();
		GLubyte* data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);

		diffuse->Bind(0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png((path + "_diffuse.png").c_str(), w, h, channels, data, w * channels);
		delete[] data;


		w = normals->GetWidth() == 1 ? 1024 : normals->GetWidth();
		h = normals->GetHeight() == 1 ? 1024 : normals->GetHeight();
		data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);
		normals->Bind(0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png((path + "_normals.png").c_str(), w, h, channels, data, w * channels);
		delete[] data;


		w = metallic->GetWidth() == 1 ? 1024 : metallic->GetWidth();
		h = metallic->GetHeight() == 1 ? 1024 : metallic->GetHeight();
		data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);
		metallic->Bind(2);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png((path + "_metallic.png").c_str(), w, h, channels, data, w * channels);
		delete[] data;


		w = roughness->GetWidth() == 1 ? 1024 : roughness->GetWidth();
		h = roughness->GetHeight() == 1 ? 1024 : roughness->GetHeight();
		data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);
		roughness->Bind(0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png((path + "_roughness.png").c_str(), w, h, channels, data, w * channels);
		delete[] data;


		w = ambientOcclusion->GetWidth() == 1 ? 1024 : ambientOcclusion->GetWidth();
		h = ambientOcclusion->GetHeight() == 1 ? 1024 : ambientOcclusion->GetHeight();
		data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);
		ambientOcclusion->Bind(0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png((path + "_ambientOcclusion.png").c_str(), w, h, channels, data, w * channels);
		delete[] data;
	}

}
