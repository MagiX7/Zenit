#include "EditorLayer.h"
#include "EditorLayer.h"
#include "EditorLayer.h"
#include "Zenit/Core/Log.h"

#include "Helpers/Nodes/ColorNode.h"
#include "Helpers/Nodes/PerlinNoiseNode.h"

#include <ImGui/imgui.h>
#include <stb_image/stb_image_write.h>

namespace Zenit {

	EditorLayer::EditorLayer()
	: camera(PerspectiveCamera({ 0,0,2 }, { 0,0,0 }, 60.0f, 1280.0f / 720.0f))
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		fbo = std::make_unique<FrameBuffer>(1280, 720, 0);
		//fbo = std::make_unique<FrameBuffer>(2048, 2048, 1);

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

		config = ed::Config();
		config.SettingsFile = "Settings/NodeEditor.json";
		context = ed::CreateEditor(&config);
		ed::SetCurrentEditor(context);
	}

	void EditorLayer::OnDetach()
	{
		ed::DestroyEditor(context);

		for (auto n : nodes)
		{
			delete n;
			n = nullptr;
		}
		nodes.clear();
	}

	void EditorLayer::OnUpdate(const TimeStep ts)
	{
		camera.Update(ts);
		panelViewport.OnUpdate(ts, model, camera);

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

	void EditorLayer::OnEvent(Event& e)
	{
	}

	void EditorLayer::HandleNodes()
	{
		ImGui::Begin("Node editor");

		if (ImGui::IsWindowHovered())
		{
			if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_RIGHT))
				showCreationPopup = true;
			else if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
				showCreationPopup = false;
		}

		if (showCreationPopup)
		{
			ImGui::OpenPopup("CreationPopup");
			if (ImGui::BeginPopup("CreationPopup"))
			{
				if (ImGui::BeginMenu("Basics"))
				{
					if (ImGui::MenuItem("Flat Color"))
					{
						CreateFlatColorNode("Flat Color", { 1,0,0 });
						showCreationPopup = false;
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Filters"))
				{
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Generators"))
				{
					if (ImGui::MenuItem("Perlin Noise"))
					{
						CreatePerlinNoiseNode("Perlin Noise");
						showCreationPopup = false;
					}
					ImGui::EndMenu();
				}
				ImGui::EndPopup();
			}
			ImGui::CloseCurrentPopup();
		}


		// NODES WORKSPACE =======================================================
		ed::Begin("");
		DrawNodes();
		ed::End();
		// NODES WORKSPACE =======================================================

		ImGui::End();
	}

	void EditorLayer::DrawNodes()
	{
		for (auto n : nodes)
		{
			ed::BeginNode(n->id);
			ImGui::Text(n->name.c_str());
			ImGui::NewLine();

			for (auto& input : n->inputs)
			{
				ed::BeginPin(input.id, input.kind);
					ImGui::Text(input.name.c_str());
				ed::EndPin();
			}
			
			ImGui::SameLine();

			switch(n->type)
			{
				case NodeType::FLAT_COLOR:
				{
					const auto node = (ColorNode*)n;
					ImGui::SetNextItemWidth(150);
					constexpr ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
					ImGui::ColorPicker3("Color", glm::value_ptr(node->color), flags);
					n = node;
					break;
				}
				case NodeType::PERLIN_NOISE:
				{
					const auto node = (PerlinNoiseNode*)n;
					ImGui::Image((ImTextureID*)node->noiseTex->GetId(), { 50,50 });
					break;
				}
				default:
					break;
			}

			ImGui::SameLine();

			for (auto& output : n->outputs)
			{
				ed::BeginPin(output.id, output.kind);
					ImGui::Text(output.name.c_str());
				ed::EndPin();
			}

			ed::EndNode();
		}

		if (ed::BeginCreate())
		{
			ed::PinId inputPinId, outputPinId;
			if (ed::QueryNewLink(&inputPinId, &outputPinId))
			{
				if (inputPinId && outputPinId) // both are valid, let's accept link
				{
					// ed::AcceptNewItem() return true when user release mouse button.
					if (ed::AcceptNewItem())
					{
						static int linkId = 100;
						// Since we accepted new link, lets add one to our list of links.
						links.push_back({ ed::LinkId(linkId++), inputPinId, outputPinId });

						// Draw new link.
						ed::Link(links.back().id, links.back().inputId, links.back().outputId);
					}
				}
			}
		}
		ed::EndCreate();

		if (ed::BeginDelete())
		{
			// There may be many links marked for deletion, let's loop over them.
			ed::LinkId deletedLinkId;
			while (ed::QueryDeletedLink(&deletedLinkId))
			{
				// If you agree that link can be deleted, accept deletion.
				if (ed::AcceptDeletedItem())
				{
					// Then remove link from your data.
					for (int i = 0; i < links.size(); ++i)
					{
						if (links[i].id == deletedLinkId)
						{
							links.erase(links.begin() + i);
							break;
						}
					}
				}
			}
		}
		ed::EndDelete();
	}

	Node* EditorLayer::FindNode(ed::NodeId id)
	{
		for (auto node : nodes)
			if (node->id == id)
				return node;

		return nullptr;
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

	Node* EditorLayer::CreateFlatColorNode(const char* name, const glm::vec3& color)
	{
		ColorNode* node = new ColorNode(creationId++, name, NodeType::FLAT_COLOR, color);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin pin = Pin(creationId++, "Red", PinType::Float, ed::PinKind::Output);
		pin.node = node;
		node->outputs.emplace_back(pin);

		Pin pin2 = Pin(creationId++, "Green", PinType::Float, ed::PinKind::Output);
		pin2.node = node;
		node->outputs.emplace_back(pin2);

		Pin pin3 = Pin(creationId++, "Blue", PinType::Float, ed::PinKind::Output);
		pin3.node = node;
		node->outputs.emplace_back(pin3);

		return nodes.back();
	}

	Node* EditorLayer::CreatePerlinNoiseNode(const char* name)
	{
		PerlinNoiseNode* node = new PerlinNoiseNode(creationId++, name, NodeType::PERLIN_NOISE);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "Input", PinType::Delegate, ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin output = Pin(creationId++, "Output", PinType::Delegate, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		node->noiseShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/perlin_noise.shader");
		node->noiseTex = std::make_unique<Texture2D>(nullptr, 1024, 1024);

		node->noiseTex->BindImage();
		node->noiseShader->Bind();
		node->noiseShader->SetUniform1i("imgOutput", 0);
		glDispatchCompute(node->noiseTex->GetWidth() / 8, node->noiseTex->GetHeight() / 4, 1); // 8 and 4 is bc of the shader file
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		return nodes.back();
	}
}
