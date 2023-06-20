#include "CircleNode.h"

namespace Zenit {

	CircleNode::CircleNode(int id, const char* name)
		: Node(id, name), radius(0.3), blur(0.001)
	{
		type = NodeType::CIRCLE;
		offset = { 0,0 };

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Generators/circle.shader");
		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);
	}

	CircleNode::~CircleNode()
	{
	}

	void CircleNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();
		computeShader->SetUniform1f("radius", radius);
		computeShader->SetUniform1f("blur", blur);
		computeShader->SetUniformVec2f("offset", offset);
		DispatchCompute(1, 1);

		Node::Update(ts);

		regenerate = false;
	}

	void CircleNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void CircleNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		bool changedOffset = ImGui::DragFloat2("Offset", glm::value_ptr(offset), 0.01);
		bool changedRadius = ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, 1.0f);
		bool changedBlur = ImGui::DragFloat("Blur", &blur, 0.01f, 0.0f, 1.0f);

		if (changedOffset || changedRadius || changedBlur)
			regenerate = true;

		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	SerializerValue CircleNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		Node::SaveCore(object);
		JSONSerializer::SetVector2f(object, "offset", offset);
		JSONSerializer::SetNumber(object, "radius", radius);
		JSONSerializer::SetNumber(object, "blur", blur);

		return value;
	}

	void CircleNode::Load(SerializerObject& obj)
	{
		offset = JSONSerializer::GetVector2fFromObject(obj, "offset");
		radius = JSONSerializer::GetNumberFromObject(obj, "radius");
		blur = JSONSerializer::GetNumberFromObject(obj, "blur");
	}
}