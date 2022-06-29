#include "EditorLayer.h"
#include "EditorLayer.h"
#include "EditorLayer.h"
#include "Zenit/Core/Log.h"

#include "Helpers/Nodes/ColorNode.h"
#include "Helpers/Nodes/ComputeShaderNode.h"
#include "Helpers/Math.h"

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
		diffuse = new Texture2D(&data, 1, 1);
		normal = new Texture2D(&data, 1, 1);
		metallic = new Texture2D(&data, 1, 1);
		roughness = new Texture2D(&data, 1, 1);
		ambientOcclusion = new Texture2D(&data, 1, 1);

		dirLight = DirectionalLight();
		skyboxProps = SkyboxProperties();

		config = ed::Config();
		config.SettingsFile = "Settings/NodeEditor.json";
		config.DragButtonIndex = 2;
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

		ImGui::Image((void*)diffuse->GetId(), { 100,100 });

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
			{
				rightClickedNodeId = ed::GetHoveredNode() ? showNodePopup = true : showCreationPopup = true;
			}
			else if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
			{
				showCreationPopup = false;
				showNodePopup = false;
			}
		}

		if (showCreationPopup)
		{
			ShowNodeCreationPopup();
		}

		if (showNodePopup)
		{
			ShowNodeOptionsPopup();
		}


		// NODES WORKSPACE =======================================================
		ed::Begin("");
			DrawNodes();
		ed::End();
		// NODES WORKSPACE =======================================================

		ImGui::End();
	}

	void EditorLayer::HandleLinks()
	{
		if (ed::BeginCreate())
		{
			ed::PinId inputPinId, outputPinId;
			if (ed::QueryNewLink(&inputPinId, &outputPinId))
			{
				if (inputPinId && outputPinId)
				{
					Pin startPin = *FindPin(inputPinId);
					Pin endPin = *FindPin(outputPinId);
					
					if (startPin.kind == ed::PinKind::Input)
					{
						std::swap(startPin, endPin);
						std::swap(inputPinId, outputPinId);
					}

					if (!startPin.id.Invalid && !endPin.id.Invalid)
					{
						if (endPin.id == startPin.id)
						{
							ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
						}

						else if (endPin.kind == startPin.kind)
						{
							ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
						}

						else if (endPin.type != startPin.type)
						{
							ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
						}

						else if (ed::AcceptNewItem())
						{
							static int linkId = 100;
							links.push_back({ ed::LinkId(linkId++), inputPinId, outputPinId });

							if (endPin.node->type == NodeType::TEXTURE)
							{
								const auto n = (ComputeShaderNode*)endPin.node;
								n->BindCoreData();
								if (startPin.node->type == NodeType::FLAT_COLOR)
								{
									const auto inNode = (ColorNode*)startPin.node;
									n->computeShader->SetUniformVec3f("inputColor", inNode->color);
								}
								n->DispatchCompute(8, 4);
							}
						}
					}					
				}
			}
		}
		ed::EndCreate();

		if (ed::BeginDelete())
		{
			ed::LinkId deletedLinkId;
			while (ed::QueryDeletedLink(&deletedLinkId))
			{
				if (ed::AcceptDeletedItem())
				{
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
					if (diffuseOutput == node)
					{
						uint32_t data = Math::GetRGBAHexadecimal(node->color);
						diffuse->SetData(&data);
					}
					n = node;
					break;
				}
				case NodeType::TEXTURE:
				{
					const auto node = (ComputeShaderNode*)n;
					ImGui::Image((ImTextureID*)node->texture->GetId(), { 50,50 });
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

		// Links creation
		HandleLinks();

		// Links Drawing
		for (const auto& link : links)
			ed::Link(link.id, link.inputId, link.outputId);

	}

	Node* EditorLayer::FindNode(ed::NodeId id) const
	{
		for (const auto node : nodes)
		{
			if (node->id == id)
				return node;
		}

		return nullptr;
	}

	Pin* EditorLayer::FindPin(ed::PinId id) const
	{
		for (int i = 0; i < nodes.size(); ++i)
		{
			for (auto inputPin : nodes[i]->inputs)
			{
				if (inputPin.id == id)
					return &inputPin;
			}

			for (auto outputPin : nodes[i]->outputs)
			{
				if (outputPin.id == id)
					return &outputPin;
			}
		}

		return nullptr;
	}

	void EditorLayer::SetDiffuseData()
	{
		if (!diffuseOutput)
			return;

		switch (diffuseOutput->type)
		{
			case NodeType::TEXTURE:
			{
				const auto node = (ComputeShaderNode*)diffuseOutput;
				diffuse = node->texture.get();
				break;
			}
			case NodeType::FLAT_COLOR:
			{
				const auto node = (ColorNode*)diffuseOutput;
				uint32_t data = Math::GetRGBAHexadecimal(node->color);
				diffuse->SetData(&data);

				break;
			}
		}
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
		std::string path = FileDialog::SaveFile("png (*.png)\0*.png\0");
		if (path.empty())
			return;

		constexpr int channels = 4;
		const int w = diffuse->GetWidth() == 1 ? 1024 : diffuse->GetWidth();
		const int h = diffuse->GetHeight() == 1 ? 1024 : diffuse->GetHeight();
		GLubyte* data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);

		diffuse->Bind(0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png((path + "_diffuse.png").c_str(), w, h, channels, data, w * channels);
		delete[] data;


		data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);
		normal->Bind(0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png((path + "_normals.png").c_str(), w, h, channels, data, w * channels);
		delete[] data;


		data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);
		metallic->Bind(0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png((path + "_metallic.png").c_str(), w, h, channels, data, w * channels);
		delete[] data;


		data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);
		roughness->Bind(0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png((path + "_roughness.png").c_str(), w, h, channels, data, w * channels);
		delete[] data;


		data = new GLubyte[channels * w * h];
		memset(data, 0, channels * w * h);
		ambientOcclusion->Bind(0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_flip_vertically_on_write(1);
		stbi_write_png((path + "_ambientOcclusion.png").c_str(), w, h, channels, data, w * channels);
		delete[] data;

	}

	void EditorLayer::ShowNodeCreationPopup()
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

	void EditorLayer::ShowNodeOptionsPopup()
	{
		ImGui::OpenPopup("NodePopup");
		if (ImGui::BeginPopup("NodePopup"))
		{
			if (ImGui::BeginMenu("Set as Output"))
			{
				if (rightClickedNodeId.Get() != -1)
				{
					Node* node = FindNode(rightClickedNodeId);

					if (ImGui::MenuItem("Diffuse"))
					{
						diffuseOutput = node;
						SetDiffuseData();
						showNodePopup = false;
					}
					else if (ImGui::MenuItem("Normals"))
					{
						normalsOutput = node;
						showNodePopup = false;
					}
					else if (ImGui::MenuItem("Metallic"))
					{
						metallicOutput = node;
						showNodePopup = false;
					}
					else if (ImGui::MenuItem("Roughness"))
					{
						roughnessOutput = node;
						showNodePopup = false;
					}
					else if (ImGui::MenuItem("Ambient Occlusion"))
					{
						aoOutput = node;
						showNodePopup = false;
					}
				}

				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		ImGui::CloseCurrentPopup();
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
		ComputeShaderNode* node = new ComputeShaderNode(creationId++, name, NodeType::TEXTURE);
		node->size = { 5,5 };
		nodes.emplace_back(node);

		Pin input = Pin(creationId++, "Input", PinType::Float, ed::PinKind::Input);
		input.node = node;
		node->inputs.emplace_back(input);

		Pin output = Pin(creationId++, "Output", PinType::Object, ed::PinKind::Output);
		output.node = node;
		node->outputs.emplace_back(output);

		node->computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/perlin_noise.shader");
		node->texture = std::make_unique<Texture2D>(nullptr, 1024, 1024);

		node->BindCoreData();
		node->computeShader->SetUniformVec3f("inputColor", { 1,1,1 });
		node->DispatchCompute(8, 4);

		return nodes.back();
	}
}
