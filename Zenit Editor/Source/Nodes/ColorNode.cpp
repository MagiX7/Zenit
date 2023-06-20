#include "ColorNode.h"

namespace Zenit {
	ColorNode::ColorNode(int id, const char* name,  const glm::vec3& color)
		: Node(id, name), color(color)
	{
		type = NodeType::COLOR;
		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/color.shader");
		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);
	}

	void ColorNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();
		computeShader->SetUniformVec3f("color", color);
		DispatchCompute(1, 1);
		
		Node::Update(ts);

		regenerate = false;
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
		
		/*JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);*/
		Node::SaveCore(object);
		JSONSerializer::SetVector3f(object, "color", color);

		return value;
	}

	void ColorNode::Load(SerializerObject& obj)
	{
		color = JSONSerializer::GetVector3fFromObject(obj, "color");
	}
}
