#include "EditorLayer.h"
#include "Zenit/Core/Log.h"

#include "Nodes/ColorNode.h"
#include "Nodes/NoiseNode.h"
#include "Nodes/VoronoiNode.h"
#include "Nodes/Constants/Vec1Node.h"
#include "Helpers/Math.h"

#include <ImGui/imgui.h>
#include <stb_image/stb_image_write.h>

#include <filesystem>


#define SKYBOX_PATH "\\Assets\\Skybox\\"
#define MODELS_PATH "\\Assets\\Models\\"

namespace Zenit {

	static std::mutex skyboxesMutex;

	static void LoadMeshes(std::vector<Model*>* models, std::string path)
	{
		std::lock_guard<std::mutex> lock(skyboxesMutex);
		models->push_back(ModelImporter::ImportModel(path));
		ZN_INFO("Model loaded");
	}


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

		LoadSkyboxes();
		
		LoadModels();
		//currentModel = ModelImporter::ImportModel("Assets/Models/Primitives/cubo.obj");

		pbrShader = std::make_unique<Shader>("Assets/Shaders/pbr.shader");
		
		uint32_t data = 0xffffffff;
		diffuse = new Texture2D(&data, 1, 1);
		normals = new Texture2D(&data, 1, 1);
		metallic = new Texture2D(&data, 1, 1);
		roughness = new Texture2D(&data, 1, 1);
		ambientOcclusion = new Texture2D(&data, 1, 1);
		
		white = std::make_shared<Texture2D>("Settings/white.png");

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

		delete panelNodes;

		for (int i = 0; i < skyboxes.size(); ++i)
		{
			delete skyboxes[i];
		}
		skyboxes.clear();
	}

	void EditorLayer::OnUpdate(const TimeStep ts)
	{
		camera.Update(ts);
		panelViewport.OnUpdate(ts, currentModel, camera);
		panelNodes->Update(ts);

		fbo->Bind();
		{
			Renderer3D::Clear({ 0.05,0.05,0.05,1 });
			
			if (skyboxProps.draw)
				currentSkybox->Draw(glm::mat3(camera.GetView()), camera.GetProjection());

			if (currentModel)
			{
				pbrShader->Bind();
				SetModelShaderData();
				currentModel->Draw();
				pbrShader->Unbind();
			}
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
		if (ImGui::BeginMenu("3D Model"))
		{
			for (const auto& model : models)
			{
				if (ImGui::MenuItem(model->GetName().c_str()))
				{
					currentModel = model;
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Refresh"))
			{
				ReloadModels();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Skybox"))
		{
			for (const auto& skybox : skyboxes)
			{
				if (ImGui::MenuItem(skybox->GetName().c_str()))
				{
					currentSkybox = skybox;
				}
			}

			ImGui::Separator();
			
			if (ImGui::MenuItem("Refresh"))
			{
				ReloadSkyboxes();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();


		ImGui::Begin("Lightning Settings");
		{
			ImGui::Text("Directional Light");
			ImGui::Text("Direction");
			ImGui::SameLine();
			ImGui::DragFloat3("##Direction", glm::value_ptr(dirLight.dir), 0.1f);
			
			ImGui::Text("Intensity");
			ImGui::SameLine();
			ImGui::DragFloat("##Intensity", &dirLight.intensity, 0.05f, 0.0f);

			ImGui::SetNextItemWidth(200);
			ImGui::ColorEdit3("Color", glm::value_ptr(dirLight.color));

			ImGui::Dummy({ 0,10 });
			ImGui::Separator();
			ImGui::Dummy({ 0,10 });

			ImGui::Text("Skybox");
			ImGui::Checkbox("Draw Skybox", &skyboxProps.draw);

			if (skyboxProps.draw)
			{
				ImGui::Checkbox("Enable Reflection", &currentSkybox->IsReflectionEnabled());
				if (currentSkybox->IsReflectionEnabled())
				{
					ImGui::DragFloat("Reflection Blur", &currentSkybox->GetReflectionLod(), 0.05f, 0.0f, 4.5f);
				}
			}
		}
		ImGui::End();

		ImGui::Begin("Final Textures");
		{			
			ImGui::Image((ImTextureID)diffuse->GetId(), { 200,200 }, { 0,1 }, { 1,0 });
			ImGui::Image((ImTextureID)normals->GetId(), { 200,200 }, { 0,1 }, { 1,0 });
			ImGui::Image((ImTextureID)metallic->GetId(), { 200,200 }, { 0,1 }, { 1,0 });
			ImGui::Image((ImTextureID)roughness->GetId(), { 200,200 }, { 0,1 }, { 1,0 });
		}
		ImGui::End();

		ImGui::Begin("Performance");
		{
			ImGui::Text(std::to_string(Application::GetInstance().GetTimeStep()).c_str());
		}
		ImGui::End();


		panelViewport.OnImGuiRender(fbo.get(), camera);
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
			if (diffuse != white.get())
				diffuse = white.get();
			return false;
		}

		const auto n = (ComputeShaderNode*)node;
		diffuse = n->texture.get();
		return true;
	}

	bool EditorLayer::SetNormalsData(Node* node)
	{
		if (!node)
		{
			if (normals != white.get())
				normals = white.get();
			return false;
		}

		const auto n = (ComputeShaderNode*)node;
		normals = n->texture.get();
		return true;
	}

	bool EditorLayer::SetMetallicData(Node* node)
	{
		if (!node)
		{
			if (metallic != white.get())
				metallic = white.get();
			return false;
		}

		const auto n = (ComputeShaderNode*)node;
		metallic = n->texture.get();
		return true;
	}

	bool EditorLayer::SetRoughnessData(Node* node)
	{
		if (!node)
		{
			if (roughness != white.get())
				roughness = white.get();
			return false;
		}

		switch (node->outputType)
		{
			case NodeOutputType::TEXTURE:
			{
				const auto n = (ComputeShaderNode*)node;
				roughness = n->texture.get();
				return true;
			}
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

	void EditorLayer::SetModelShaderData()
	{
		pbrShader->SetUniformMatrix4f("view", camera.GetView());
		pbrShader->SetUniformMatrix4f("projection", camera.GetProjection());

		pbrShader->SetUniformMatrix4f("model", currentModel->GetTransform());
		pbrShader->SetUniformVec3f("camPos", camera.GetPosition());

		pbrShader->SetUniformVec3f("dirLight.direction", glm::normalize(dirLight.dir));
		pbrShader->SetUniformVec3f("dirLight.color", dirLight.color);
		pbrShader->SetUniform1f("dirLight.intensity", dirLight.intensity);

		diffuse->Bind(0);
		pbrShader->SetUniform1i("diffuseTexture", 0);

		normals->Bind(1);
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
			currentSkybox->BindIrradianceMap(5);
			pbrShader->SetUniform1i("irradianceMap", 5);
		
			currentSkybox->BindPrefilterMap(6);
			pbrShader->SetUniform1i("skyboxPrefilterMap", 6);
		
			currentSkybox->BindBRDF(7);
			pbrShader->SetUniform1i("skyboxBrdf", 7);
		
			pbrShader->SetUniform1f("reflectionLod", currentSkybox->GetReflectionLod());
			pbrShader->SetUniform1i("skyboxReflectionEnabled", currentSkybox->IsReflectionEnabled());
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

	
	
	void EditorLayer::LoadSkyboxes()
	{
		std::filesystem::path cp = std::filesystem::current_path();
		if (std::filesystem::exists(SKYBOX_PATH))
		{
			ZN_ERROR("Skybox Folder couldn't be located");
			return;
		}

		std::filesystem::path absolutePath = std::filesystem::current_path().string() + SKYBOX_PATH;
		
		for (std::filesystem::recursive_directory_iterator it(absolutePath), end; it != end; ++it)
		{
			if (!std::filesystem::is_directory(it->path()))
			{
				std::string path = it->path().string();
				//futures.push_back(std::async(std::launch::async, LoadMeshes, &skyboxes, path));
				skyboxes.push_back(new Skybox(path));
			}
		}
		currentSkybox = skyboxes[0];

	}

	void EditorLayer::ReloadSkyboxes()
	{
		for (int i = 0; i < skyboxes.size(); ++i)
		{
			delete skyboxes[i];
		}
		skyboxes.clear();

		LoadSkyboxes();
	}

	void EditorLayer::LoadModels()
	{
		std::filesystem::path cp = std::filesystem::current_path();
		if (std::filesystem::exists(MODELS_PATH))
		{
			ZN_ERROR("3D Models Folder couldn't be located");
			return;
		}

		std::filesystem::path absolutePath = std::filesystem::current_path().string() + MODELS_PATH;

		for (std::filesystem::recursive_directory_iterator it(absolutePath), end; it != end; ++it)
		{
			if (!std::filesystem::is_directory(it->path()))
			{
				std::string path = it->path().string();
				//futures.push_back(std::async(std::launch::async, LoadMeshes, &skyboxes, path));
				if (Model* model = ModelImporter::ImportModel(path))
				{
					models.push_back(model);
					if (model->GetName() == "Sphere")
					{
						currentModel = model;
					}
				}
			}
		}
		
		if (!currentModel)
			currentModel = models[0];
	}

	void EditorLayer::ReloadModels()
	{
		for (int i = 0; i < models.size(); ++i)
		{
			delete models[i];
		}
		models.clear();

		LoadModels();
	}

}
