#include "VoronoiNode.h"

namespace Zenit {

	VoronoiNode::VoronoiNode(int id, const char* name)
		: Node(id, name)
	{
		type = NodeType::VORONOI;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Generators/voronoi.shader");
		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);

		latestSeed = 1.54;
		scale = 5;
	}

	VoronoiNode::~VoronoiNode()
	{
	}

	void VoronoiNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();
		computeShader->SetUniform1f("seed", latestSeed);
		computeShader->SetUniform1i("scale", scale);
		latestSeed += Application::GetInstance().GetTotalExecutionTime();
		DispatchCompute(1, 1);
		
		regenerate = false;
	}

	void VoronoiNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void VoronoiNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		ImGui::Dummy({ 0, 10 });

		bool changedScale = ImGui::DragInt("Scale", &scale, 0.1f);

		if (ImGui::Button("Regenerate") || changedScale)
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
		JSONSerializer::SetNumber(object, "seed", latestSeed);
		JSONSerializer::SetNumber(object, "scale", scale);

		return value;
	}

	void VoronoiNode::Load(SerializerObject& obj)
	{
		latestSeed = JSONSerializer::GetNumberFromObject(obj, "seed");
		scale = JSONSerializer::GetNumberFromObject(obj, "scale");
	}
}
