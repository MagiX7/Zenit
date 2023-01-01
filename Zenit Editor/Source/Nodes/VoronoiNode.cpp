#include "VoronoiNode.h"

namespace Zenit {

	VoronoiNode::VoronoiNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType), brightness(1.0f)
	{
		type = NodeType::VORONOI;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/voronoi.shader");
		texture = std::make_shared<Texture2D>(nullptr, 512, 512);

		latestSeed = 1.54;
		zoom = 5;
	}

	VoronoiNode::~VoronoiNode()
	{
	}

	void VoronoiNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();
		computeShader->SetUniform1f("brightness", brightness);
		computeShader->SetUniform1f("zoom", zoom);
		computeShader->SetUniform1f("seed", latestSeed);
		latestSeed += Application::GetInstance().GetTotalExecutionTime();
		regenerate = false;
		DispatchCompute(1, 1);
	}

	void VoronoiNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void VoronoiNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		ImGui::Dummy({ 0, 10 });

		bool changedBrightness = ImGui::DragFloat("Brightness", &brightness, 0.01f, 0.0f, 1.0f);
		bool changedZoom = ImGui::DragFloat("Zoom", &zoom, 0.1f, 0.0f);

		if (ImGui::Button("Regenerate") || changedBrightness || changedZoom)
			regenerate = true;

		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0, 1 }, { 1,0 });
	}

	SerializerValue VoronoiNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetNumber(object, "brightness", brightness);
		JSONSerializer::SetNumber(object, "zoom", zoom);
		JSONSerializer::SetNumber(object, "seed", latestSeed);

		return value;
	}

	void VoronoiNode::Load(SerializerObject& obj)
	{
		brightness = JSONSerializer::GetNumberFromObject(obj, "brightness");
		zoom = JSONSerializer::GetNumberFromObject(obj, "zoom");
		latestSeed = JSONSerializer::GetNumberFromObject(obj, "seed");
	}
}
