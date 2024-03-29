#include "TransformNode.h"

namespace Zenit {

	TransformNode::TransformNode(int id, const char* name)
		: Node(id, name)
	{
		type = NodeType::TRANSFORM;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/transform.shader");
		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);
		inputTexture = GetWhite();
	}

	TransformNode::~TransformNode()
	{
	}

	void TransformNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();

		inputTexture->Bind(1);
		computeShader->SetUniform1i("inputTexture", 1);
		computeShader->SetUniformVec2f("offset", offset);
		computeShader->SetUniform1f("angle", rotationAngle);
		computeShader->SetUniform1i("scale", scale);

		DispatchCompute(1, 1);

		Node::Update(ts);

		regenerate = false;
	}

	void TransformNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void TransformNode::OnImGuiInspectorRender()
	{
		if (ImGui::DragFloat2("Offset", glm::value_ptr(offset), 0.01f))
			regenerate = true;

		if (ImGui::DragFloat("Rotation Angle", &rotationAngle, 0.1f))
			regenerate = true;

		if (ImGui::DragInt("Scale", &scale, 0.01f))
			regenerate = true;

		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	void TransformNode::SetInputTexture(Texture2D* texture)
	{
		inputTexture = texture;
		regenerate = true;
	}

	SerializerValue TransformNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		Node::SaveCore(object);
		JSONSerializer::SetVector2f(object, "offset", offset);
		JSONSerializer::SetNumber(object, "rotationAngle", rotationAngle);
		JSONSerializer::SetNumber(object, "scale", scale);
		

		return value;
	}

	void TransformNode::Load(SerializerObject& obj)
	{
		offset = JSONSerializer::GetVector2fFromObject(obj, "offset");
		rotationAngle = JSONSerializer::GetNumberFromObject(obj, "rotationAngle");
		scale = JSONSerializer::GetNumberFromObject(obj, "scale");
	}

}