#include "ColorNode.h"

namespace Zenit {
	ColorNode::ColorNode(int id, const char* name, NodeOutputType outputType, const glm::vec3& color)
		: ComputeShaderNode(id, name, outputType), color(color)
	{
		type = NodeType::COLOR;
		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/color.shader");
		texture = std::make_shared<Texture2D>(nullptr, 512, 512);
	}

	void ColorNode::Update(TimeStep)
	{
		if (!regenerate)
			return;

		BindCoreData();
		computeShader->SetUniformVec3f("color", color);
		DispatchCompute(1, 1);
	}

	void ColorNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void ColorNode::OnImGuiInspectorRender()
	{
		ImGui::SetNextItemWidth(256);
		if (ImGui::ColorPicker3("Color", glm::value_ptr(color)))
			regenerate = true;
	}

	SerializerValue ColorNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetVector3f(object, "color", color);

		return value;
	}
}
