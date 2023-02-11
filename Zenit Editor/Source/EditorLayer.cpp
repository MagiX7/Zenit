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
#include <queue>


#define SKYBOX_PATH "\\Assets\\Skybox\\"
#define MODELS_PATH "\\Assets\\Models\\"
#define SAVE_PATH   "Settings/SavedData.json"

namespace Zenit {

	static std::mutex skyboxesMutex;

	static void LoadMeshes(std::vector<Model*>* models, std::string path)
	{
		std::lock_guard<std::mutex> lock(skyboxesMutex);
		models->push_back(ModelImporter::ImportModel(path));
		ZN_INFO("Model loaded");
	}


	EditorLayer::EditorLayer() : camera(PerspectiveCamera({ 0,0,2.5 }, { 0,0,0 })), frustum({ camera })
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

		white = std::make_shared<Texture2D>("Settings/white.png");
		panelNodes = new PanelNodes(this);
		if (!Load())
		{
			uint32_t data = 0xffffffff;
			diffuse = new Texture2D(&data, 1, 1);
			diffuse->SetName("white");
			normals = new Texture2D(&data, 1, 1);
			normals->SetName("white");
			metallic = new Texture2D(&data, 1, 1);
			metallic->SetName("white");
			roughness = new Texture2D(&data, 1, 1);
			roughness->SetName("white");
			ambientOcclusion = new Texture2D(&data, 1, 1);
		}		

		pbrShader = std::make_unique<Shader>("Assets/Shaders/pbr.shader");
		dirLight = DirectionalLight();
		skyboxProps = SkyboxProperties();

		FocusCameraOnModel();

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
		//camera.Update(ts);
		panelViewport.OnUpdate(ts, currentModel, camera);
		panelNodes->Update(ts);

		fbo->Bind();
		{
			Renderer3D::GetInstance()->Clear({ 0.05,0.05,0.05,1 });
			
			if (skyboxProps.draw)
				currentSkybox->Draw(glm::mat3(camera.GetViewMatrix()), camera.GetProjectionMatrix());

			if (currentModel)
			{
				pbrShader->Bind();
				SetModelShaderData();
				currentModel->Draw(pbrShader);
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
			else if (ImGui::MenuItem("Save"))
			{
				Save();
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
					FocusCameraOnModel();
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

		ImGui::Begin("Hierarchy");
		{
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				currentMesh = nullptr;

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx(currentModel->GetName().c_str(), flags | (currentMesh ? 0 : ImGuiTreeNodeFlags_Selected));
			
			if (ImGui::IsItemClicked())
			{
				// Set current mesh to 
				currentMesh = nullptr;
			}

			if (opened)
			{
				std::queue<Mesh*> q;
				for(auto mesh : currentModel->GetMeshes())
					q.push(mesh);

				while (!q.empty())
				{
					auto& curr = q.front();
					q.pop();

					bool opened = ImGui::TreeNodeEx(curr->GetName().c_str(), flags | (currentMesh == curr ? ImGuiTreeNodeFlags_Selected : 0));
					
					if (ImGui::IsItemClicked())
					{
						// Set current mesh to 
						currentMesh = curr;
					}
					
					if (opened)
					{
						ImGui::TreePop();
					}
				}

				ImGui::TreePop();
			}
		}
		ImGui::End();

		ImGui::Begin("Inspector");
		{
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				// TODO: Model Inspector UI + delete all orthographic stuff
				if (camera.GetProjectionType() == PerspectiveCamera::ProjectionType::PERSPECTIVE)
				{
					ImGui::Dummy({ 0,5 });

					auto pos = camera.GetPosition();
					if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.1f))
					{
						camera.SetPosition(pos);
					}
					if (ImGui::Button("Reset"))
					{
						camera.SetPosition(glm::vec3(0, 0, 2));
					}

					ImGui::Dummy({ 0,2.5f });

					auto rot = camera.GetRotation();
					if (ImGui::DragFloat3("Rotation", glm::value_ptr(rot), 0.1f))
					{
						camera.SetRotation(rot);
					}
					if (ImGui::Button("Reset##2"))
					{
						camera.SetRotation(glm::vec3(0, 0, 0));
					}

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
			
			if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
			{
				glm::vec3 pos = currentModel->GetTransform()[3];

				if (ImGui::DragFloat3("Position##", glm::value_ptr(pos), 0.1f))
				{
					currentModel->SetPosition(pos);
				}

				if (currentModel && ImGui::Button("Reset Rotation"))
					currentModel->ResetRotation();
			}

			ImGui::Dummy({ 0, 3 });
			ImGui::Separator();
			ImGui::Dummy({ 0, 3 });


			if (currentMesh)
			{
				if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
				{
					//if (ImGui::Button("Apply textures"))
					{
						/*currentMesh->SetDiffuse(diffuse);
						currentMesh->SetNormals(normals);
						currentMesh->SetMetallic(metallic);
						currentMesh->SetRoughness(roughness);*/
					}
				}
			}
			else
			{
				ImGui::BulletText("There is no selected mesh.");
				ImGui::Text("         Textures will be applied to every mesh inside the model.");
			}


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
			ImGui::Text(std::to_string(Application::GetInstance().GetTimeStep()).c_str());
			ImGui::Text(std::to_string(Application::GetInstance().GetTotalExecutionTime()).c_str());
		}
		ImGui::End();
		


		panelViewport.OnImGuiRender(fbo.get(), camera);
		//panelLayerStack.OnImGuiRender(layers);
		panelNodes->OnImGuiRender();


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
		diffuse->SetName(n->name + "_" + std::to_string(n->id.Get()));
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
		normals->SetName(n->name + "_" + std::to_string(n->id.Get()));

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
		metallic->SetName(n->name + "_" + std::to_string(n->id.Get()));

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
				roughness->SetName(n->name + "_" + std::to_string(n->id.Get()));
				return true;
			}
			//case NodeOutputType::VEC1:
			//{
			//	const auto n = (Vec1Node*)node;
			//	roughnessValue = n->value;
			//	return true;
			//	break;
			//}
		}

		return false;
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

		//ambientOcclusion->Bind(4);
		//pbrShader->SetUniform1i("ambientOcclusionTexture", 4);


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

	void EditorLayer::Save()
	{
		serializerRootValue = JSONSerializer::CreateValue();
		SerializerObject rootObj = JSONSerializer::GetObjectWithValue(serializerRootValue);

		JSONSerializer::SetObjectValue(rootObj, "App", JSONSerializer::CreateValue());
		SerializerObject appObj = JSONSerializer::GetObjectWithName(rootObj, "App");
		
		JSONSerializer::SetString(appObj, "diffuse", diffuse->GetName().c_str());
		JSONSerializer::SetString(appObj, "normals", normals->GetName().c_str());
		JSONSerializer::SetString(appObj, "metallic", metallic->GetName().c_str());
		JSONSerializer::SetString(appObj, "roughness", roughness->GetName().c_str());
		
		panelNodes->SaveNodes(appObj);
		
		JSONSerializer::DumpFile(serializerRootValue, SAVE_PATH);
		JSONSerializer::FreeValue(serializerRootValue);
	}

	bool EditorLayer::Load()
	{
		//return false;

		serializerRootValue = JSONSerializer::ReadFile(SAVE_PATH);
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
		SetDiffuseData(panelNodes->FindNode(nodeId));

		nodeId = -1;
		start = normalsName.find_last_of("_");
		if (start != std::string::npos)
		{
			nodeId = std::stoi(normalsName.substr(start + 1));
		}
		SetNormalsData(panelNodes->FindNode(nodeId));

		nodeId = -1;
		start = metallicName.find_last_of("_");
		if (start != std::string::npos)
		{
			nodeId = std::stoi(metallicName.substr(start + 1));
		}
		SetMetallicData(panelNodes->FindNode(nodeId));

		nodeId = -1;
		start = roughnessName.find_last_of("_");
		if (start != std::string::npos)
		{
			nodeId = std::stoi(roughnessName.substr(start + 1));
		}
		SetRoughnessData(panelNodes->FindNode(nodeId));

		return true;
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

	void EditorLayer::FocusCameraOnModel()
	{
		/*glm::vec3 maxPoint = currentModel->GetAABB().GetMax();
		glm::vec3 minPoint = currentModel->GetAABB().GetMin();

		glm::vec3 h = (maxPoint - minPoint) / 2.0f;

		float angle = glm::radians(camera.GetVerticalFov() / 2);

		glm::vec3 distance = h / glm::tan(angle);

		distance.x = (distance.x + 2.5f) * camera.GetForward().x;
		distance.y = distance.y * camera.GetForward().y;
		distance.z = (distance.z + 2.5f) * camera.GetForward().z;
		glm::vec3 newPos = currentModel->GetAABB().GetCenter() - distance;
		camera.SetPosition(newPos);
		frustum.UpdateFrustum(camera);*/

		/*newUp = newFront.Cross(float3(0.0f, 1.0f, 0.0f).Cross(newFront).Normalized());
		const float meshRadius = mesh->GetLocalAABB().HalfDiagonal().Length();
		const float currentDistance = meshCenter.Distance(cameraFrustum.Pos());
		newPos = meshCenter + ((cameraFrustum.Pos() - meshCenter).Normalized() * meshRadius * 2);*/







		/*AABB modelAabb = currentModel->GetAABB();

		float verticalSize = modelAabb.GetMax().y - modelAabb.GetMin().y;

		float dist = glm::abs(verticalSize / glm::tan(camera.GetVerticalFov() / 2));

		float longest = modelAabb.GetLongestEdge();

		do
		{
			glm::vec3 camPosition = camera.GetPosition();
			glm::vec3 dir = glm::normalize(modelAabb.GetCenter() - camPosition);
			
			float totalDistance = glm::distance(modelAabb.GetCenter(), camPosition);
			if (totalDistance > dist + longest)
			{
				camPosition += dir * dist;
				camera.SetPosition(camPosition);
				frustum.UpdateFrustum(camera);
				ZN_CORE_TRACE(" >>> {0} {1} {2}", camPosition.x, camPosition.y, camPosition.z);
			}
			else if (totalDistance < dist - longest)
			{
				camPosition -= dir * dist;
				camera.SetPosition(camPosition);
				frustum.UpdateFrustum(camera);
				ZN_CORE_TRACE(" <<< {0} {1} {2}", camPosition.x, camPosition.y, camPosition.z);
			}
			else
			{
				break;
			}
		} while (!modelAabb.IsInsideFrustum(frustum));*/

		
		//reference = modelAabb->CenterPoint();
		//editorCam->SetPosition(position);
		//editorCam->Look(reference);


		


	}

	void EditorLayer::DrawHierarchyEntity()
	{


	}

}
