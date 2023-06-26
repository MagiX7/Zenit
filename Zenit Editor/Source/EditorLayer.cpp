#include "EditorLayer.h"
#include "Zenit/Core/Log.h"

#include "Nodes/ColorNode.h"
#include "Nodes/Generators/NoiseNode.h"
#include "Nodes/Generators/VoronoiNode.h"
#include "Helpers/Math.h"

#include "Helpers/NodeHelpers.h"

#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include <filesystem>


#define SKYBOX_PATH "\\Assets\\Skybox\\"
#define MODELS_PATH "\\Assets\\Models\\"
#define SAVE_PATH   "Settings/"

namespace Zenit {

	EditorLayer::EditorLayer() : camera(PerspectiveCamera({ 0,0,2 }, { 0,0,0 })), frustum({ camera })
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
		currentMesh = nullptr;

		panelNodes = new PanelNodes(this);
		
		{
			uint32_t data = 0xffffffff;
			diffuse = Node::GetWhite();
			diffuse->SetName("white");
			
			normals = new Texture2D(&data, 1, 1);
			normals->SetName("white");

			metallic = new Texture2D(&data, 1, 1);
			metallic->SetName("white");

			roughness = new Texture2D(&data, 1, 1);
			roughness->SetName("white");
		}

		pbrShader = std::make_unique<Shader>("Assets/Shaders/pbr.shader");
		dirLight = DirectionalLight();
		skyboxProps = SkyboxProperties();

		Application::GetInstance().GetWindow().SetIcon("Settings/icon.png");
	}

	void EditorLayer::OnDetach()
	{
		delete diffuse;
		delete normals;
		delete metallic;
		delete roughness;

		delete panelNodes;

		for (int i = 0; i < skyboxes.size(); ++i)
		{
			delete skyboxes[i];
		}
		skyboxes.clear();
	}

	void EditorLayer::OnUpdate(const TimeStep ts)
	{
		if (Application::GetInstance().ExitRequested())
		{
			showExitPopup = true;
		}

		if (Input::GetInstance()->IsKeyPressed(Key::KEY_LEFT_CONTROL)
			&& Input::GetInstance()->IsKeyPressed(Key::KEY_S))
		{
			Save(false);
		}

		panelViewport.OnUpdate(ts, currentModel, camera);
		panelNodes->Update(ts);

		fbo->Bind();
		{
			Renderer3D::GetInstance()->Clear({ 0.15,0.15,0.15,1 });
			
			if (skyboxProps.draw)
				currentSkybox->Draw(glm::mat3(camera.GetViewMatrix()), camera.GetProjectionMatrix());

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
			if (ImGui::MenuItem("New"))
			{
				NewScene();
			}
			else if (ImGui::MenuItem("Open..."))
			{
				Load();
			}
			else if (ImGui::MenuItem("Save"))
			{
				Save(false);
			}
			else if (ImGui::MenuItem("Save As..."))
			{
				Save(true);
			}
			else if (ImGui::MenuItem("Export"))
			{
				showExportingPanel = true;
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
		if (ImGui::BeginMenu("Render"))
		{
			bool cull = Renderer3D::GetInstance()->GetCullingValue();
			if (ImGui::Checkbox("Cull Face", &cull));
				Renderer3D::GetInstance()->SetCulling(cull);

			bool depth = Renderer3D::GetInstance()->GetDepthValue();
			if (ImGui::Checkbox("Depth testing", &depth));
				Renderer3D::GetInstance()->SetDepth(depth);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		ImGui::Begin("Lighting Settings");
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

		ImGui::Begin("Inspector");
		{
			if (ImGui::CollapsingHeader("Camera"))
			{
				if (camera.GetProjectionType() == PerspectiveCamera::ProjectionType::PERSPECTIVE)
				{
					ImGui::Dummy({ 0,5 });

					float verticalFov = camera.GetVerticalFov();
					if (ImGui::DragFloat("Vertical FOV", &verticalFov, 0.1f))
						camera.SetVerticalFov(verticalFov);

					float near = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &near, 0.1f))
						camera.SetPerspectiveNearClip(near);

					float far = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &far))
						camera.SetPerspectiveFarClip(far);

					ImGui::Dummy({ 0,5 });
				}
				else
				{
					float size = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Orthographic Size", &size))
						camera.SetOrthographicSize(size);

					float near = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &near))
						camera.SetOrthographicNearClip(near);

					float far = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &far))
						camera.SetOrthographicFarClip(far);
				}
			}
			
			ImGui::Dummy({ 0, 3 });
			ImGui::Separator();
			ImGui::Dummy({ 0, 3 });
			
			if (ImGui::CollapsingHeader("Model"))
			{
				glm::vec3 pos = currentModel->GetPosition();
				glm::vec3 rot = currentModel->GetRotation();

				if (ImGui::DragFloat3("Position##", glm::value_ptr(pos), 0.01f))
				{
					currentModel->SetPosition(pos);
				}
				if (ImGui::DragFloat3("Rotation##", glm::value_ptr(rot), 0.01f))
				{
					currentModel->SetRotation(rot);
				}

				if (currentModel && ImGui::Button("Reset##"))
					currentModel->ResetTransform();
			}

			ImGui::Dummy({ 0, 3 });
			ImGui::Separator();
			ImGui::Dummy({ 0, 3 });


			if (Node* node = panelNodes->GetSelectedNode())
				node->OnImGuiInspectorRender();
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
			ImGui::Text(("FPS: " + std::to_string(1.0f / Application::GetInstance().GetTimeStep())).c_str());
			ImGui::Text(("Delta Time: " + std::to_string(Application::GetInstance().GetTimeStep())).c_str());
		}
		ImGui::End();
		

		panelViewport.OnImGuiRender(fbo.get(), camera);
		panelNodes->OnImGuiRender();

		if (showExitPopup)
		{
			if (showExportingPanel) showExportingPanel = false;

			ImGui::OpenPopup("##Exit Popup");
			ImGui::SetNextWindowSize({ 370 , 120 });
			if (ImGui::BeginPopupModal("##Exit Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration))
			{
				ImGui::Dummy({ 0,10 });
				
				ImGui::Dummy({ 19,0 });
				ImGui::SameLine();
				ImGui::Text("Are you sure you want to exit? Remember to save.");
				
				ImGui::Dummy({ 0,20 });

				ImGui::SetNextItemWidth(256);

				ImGuiStyle& style = ImGui::GetStyle();
				float size = 100 + 100 + 25 + style.FramePadding.x * 2.0f;
				float avail = ImGui::GetContentRegionAvail().x;

				float off = (avail - size) * 0.5f;
				if (off > 0.0f)
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

				
				if (ImGui::Button("Yes", { 100,0 }))
				{
					Application::GetInstance().Terminate();
				}

				ImGui::SameLine(0, 25);
				
				if (ImGui::Button("No", { 100, 0 }))
				{
					showExitPopup = false;
				}

				ImGui::EndPopup();
			}
		}

		if (showExportingPanel)
		{
			ImGui::OpenPopup("Export Settings");
			ImGui::SetNextWindowSize({ 340 , 150});
			if (ImGui::BeginPopupModal("Export Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Dummy({ 0,2 });
				ImGui::SetNextItemWidth(256);

				static int channels = 4;
				ImGui::SliderInt("Channels", &channels, 3, 4);

				ImGui::Dummy({ 0, 5 });
				ImGui::Separator();
				ImGui::Dummy({ 0, 5 });

				
				ImGuiStyle& style = ImGui::GetStyle();
				float size = 100 + 100 + 25 + style.FramePadding.x * 2.0f;
				float avail = ImGui::GetContentRegionAvail().x;

				float off = (avail - size) * 0.5f;
				if (off > 0.0f)
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

				if (ImGui::Button("Export", { 100,0 }))
				{
					if (ExportTextures(channels))
						showExportingPanel = false;
				}

				ImGui::SameLine(0, 25);

				if (ImGui::Button("Cancel", { 100, 0 }))
				{
					channels = 4;
					showExportingPanel = false;
				}

				ImGui::EndPopup();
			}
		}

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
			diffuse = Node::GetWhite();
			return false;
		}

		const auto n = (Node*)node;
		diffuse = n->texture.get();
		diffuse->SetName(n->name + "_" + std::to_string(n->id.Get()));
		return true;
	}

	bool EditorLayer::SetNormalsData(Node* node)
	{
		if (!node)
		{
			normals = Node::GetWhite();
			return false;
		}

		const auto n = (Node*)node;
		normals = n->texture.get();
		normals->SetName(n->name + "_" + std::to_string(n->id.Get()));

		return true;
	}

	bool EditorLayer::SetMetallicData(Node* node)
	{
		if (!node)
		{
			metallic = Node::GetWhite();
			return false;
		}

		const auto n = (Node*)node;
		metallic = n->texture.get();
		metallic->SetName(n->name + "_" + std::to_string(n->id.Get()));

		return true;
	}

	bool EditorLayer::SetRoughnessData(Node* node)
	{
		if (!node)
		{
			roughness = Node::GetWhite();
			return false;
		}

		const auto n = (Node*)node;
		roughness = n->texture.get();
		roughness->SetName(n->name + "_" + std::to_string(n->id.Get()));
		
		return true;
	}

	void EditorLayer::SetModelShaderData()
	{
		pbrShader->SetUniformMatrix4f("view", camera.GetViewMatrix());
		pbrShader->SetUniformMatrix4f("projection", camera.GetProjectionMatrix());

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

	bool EditorLayer::ExportTextures(int channels)
	{
		std::string path = FileDialog::SaveFile("png (*.png)\0*.png\0");
		if (path.empty())
			return false;

		diffuse->WriteToDisk(path + "_diffuse", channels);
		normals->WriteToDisk(path + "_normals", channels);
		metallic->WriteToDisk(path + "_metallic", channels);
		roughness->WriteToDisk(path + "_roughness", channels);

		return true;
	}

	void EditorLayer::Save(bool saveAs)
	{
		std::string path = savedFilePath;
		if (saveAs || savedFilePath == "")
		{
			path = FileDialog::SaveFile("zenit (*.zenit)\0*.zenit\0");
			if (path.empty())
				return;
			
			if (!saveAs)
				savedFilePath = path;
		}


		serializerRootValue = JSONSerializer::CreateValue();
		SerializerObject rootObj = JSONSerializer::GetObjectWithValue(serializerRootValue);

		JSONSerializer::SetObjectValue(rootObj, "App", JSONSerializer::CreateValue());
		SerializerObject appObj = JSONSerializer::GetObjectWithName(rootObj, "App");
		
		JSONSerializer::SetString(appObj, "diffuse", diffuse->GetName().c_str());
		JSONSerializer::SetString(appObj, "normals", normals->GetName().c_str());
		JSONSerializer::SetString(appObj, "metallic", metallic->GetName().c_str());
		JSONSerializer::SetString(appObj, "roughness", roughness->GetName().c_str());
		
		panelNodes->SaveNodes(appObj);
		
		JSONSerializer::DumpFile(serializerRootValue, (path + ".zenit").c_str());
		JSONSerializer::FreeValue(serializerRootValue);
	}

	bool EditorLayer::Load()
	{
		std::string path = FileDialog::SaveFile("zenit (*.zenit)\0*.zenit\0");
		if (path.empty())
			return false;
		
		savedFilePath = path.substr(0, path.find_last_of("."));
		
		serializerRootValue = JSONSerializer::ReadFile(path.c_str());
		if (!serializerRootValue.value)
			return false;

		SerializerObject rootObj = JSONSerializer::GetObjectWithValue(serializerRootValue);
		SerializerObject appObj = JSONSerializer::GetObjectWithName(rootObj, "App");

		std::string diffuseName = JSONSerializer::GetStringFromObject(appObj, "diffuse");
		std::string normalsName = JSONSerializer::GetStringFromObject(appObj, "normals");
		std::string metallicName = JSONSerializer::GetStringFromObject(appObj, "metallic");
		std::string roughnessName = JSONSerializer::GetStringFromObject(appObj, "roughness");

		panelNodes->LoadNodes(appObj);

		int nodeId = -1;
		size_t start = diffuseName.find_last_of("_");
		if (start != std::string::npos)
		{			
			nodeId = std::stoi(diffuseName.substr(start + 1));
		}
		SetDiffuseData(NodeHelpers::FindNode(nodeId, panelNodes->GetNodes()));

		nodeId = -1;
		start = normalsName.find_last_of("_");
		if (start != std::string::npos)
		{
			nodeId = std::stoi(normalsName.substr(start + 1));
		}
		SetNormalsData(NodeHelpers::FindNode(nodeId, panelNodes->GetNodes()));

		nodeId = -1;
		start = metallicName.find_last_of("_");
		if (start != std::string::npos)
		{
			nodeId = std::stoi(metallicName.substr(start + 1));
		}
		SetMetallicData(NodeHelpers::FindNode(nodeId, panelNodes->GetNodes()));

		nodeId = -1;
		start = roughnessName.find_last_of("_");
		if (start != std::string::npos)
		{
			nodeId = std::stoi(roughnessName.substr(start + 1));
		}
		SetRoughnessData(NodeHelpers::FindNode(nodeId, panelNodes->GetNodes()));

		return true;
	}

	void EditorLayer::NewScene()
	{
		panelNodes->ClearNodes();
		savedFilePath = "";
		SetDiffuseData(nullptr);
		SetNormalsData(nullptr);
		SetMetallicData(nullptr);
		SetRoughnessData(nullptr);
		
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
