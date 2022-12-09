#include "ColorNode.h"

namespace Zenit {
	ColorNode::ColorNode(int id, const char* name, NodeOutputType outputType, const glm::vec3& color)
		: ComputeShaderNode(id, name, outputType), color(color)
	{
		type = NodeType::COLOR;
		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/color.shader");
		texture = std::make_unique<Texture2D>(nullptr, 1024, 1024);
	}

	void ColorNode::Update(TimeStep)
	{
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
		ImGui::ColorPicker3("Color", glm::value_ptr(color));
	}
}
