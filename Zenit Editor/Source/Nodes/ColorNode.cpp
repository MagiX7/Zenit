#include "ColorNode.h"

namespace Zenit {
	ColorNode::ColorNode(int id, const char* name, NodeOutputType outputType, const glm::vec3& color)
		: Node(id, name, outputType), color(color)
	{
		type = NodeType::COLOR;
	}

	void ColorNode::Update(TimeStep)
	{
	}

	void ColorNode::OnImGuiRender()
	{
		ImGui::SetNextItemWidth(150);
		constexpr ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
		ImGui::ColorPicker3("Color", glm::value_ptr(color), flags);
	}
}
