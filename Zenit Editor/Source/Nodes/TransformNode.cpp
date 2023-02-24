#include "TransformNode.h"

namespace Zenit {

	TransformNode::TransformNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType)
	{
		type = NodeType::TRANSFORM;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/transform.shader");
		texture = std::make_shared<Texture2D>(nullptr, 512, 512);
		inputTexture = std::make_unique<Texture2D>("Settings/white.png");
	}

	TransformNode::~TransformNode()
	{
	}

	void TransformNode::Update(TimeStep ts)
	{
		//if (!regenerate)
			//return;

		BindCoreData();

		//TODO: Revise rotation
		inputTexture->Bind(1);
		computeShader->SetUniform1i("inputTexture", 1);
		computeShader->SetUniformVec2f("offset", offset);
		computeShader->SetUniform1f("angle", rotationAngle);
		computeShader->SetUniformVec2f("scale", scale);

		DispatchCompute(1, 1);

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

		if (ImGui::DragFloat2("Scale", glm::value_ptr(scale), 0.01f))
			regenerate = true;

		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	void TransformNode::SetInputTexture(std::shared_ptr<Texture2D> texture)
	{
		inputTexture = texture;
		regenerate = true;
	}

	SerializerValue TransformNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetVector2f(object, "offset", offset);
		JSONSerializer::SetNumber(object, "rotationAngle", rotationAngle);

		return value;
	}

	void TransformNode::Load(SerializerObject& obj)
	{
		offset = JSONSerializer::GetVector2fFromObject(obj, "offset");
		rotationAngle = JSONSerializer::GetNumberFromObject(obj, "rotationAngle");
	}

}