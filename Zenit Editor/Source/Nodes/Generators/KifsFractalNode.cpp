#include "KifsFractalNode.h"

namespace Zenit {

	KifsFractalNode::KifsFractalNode(int id, const char* name)
		: Node(id, name)
	{
		type = NodeType::KIFS_FRACTAL;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Generators/kifs_fractal.shader");
		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);
	}

	KifsFractalNode::~KifsFractalNode()
	{
	}

	void KifsFractalNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();
		computeShader->SetUniform1f("angle", angle);
		computeShader->SetUniform1f("reflectionAngle", reflectionAngle);
		computeShader->SetUniform1i("iterations", iterations);
		computeShader->SetUniform1f("lineThickness", lineThickness);
		DispatchCompute(1,1);

		Node::Update(ts);

		regenerate = false;
	}

	void KifsFractalNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void KifsFractalNode::OnImGuiInspectorRender()
	{
		if (ImGui::DragFloat("Angle", &angle, 0.01f))
			regenerate = true;

		if (ImGui::DragFloat("Reflection Angle", &reflectionAngle, 0.01f))
			regenerate = true;

		if (ImGui::DragInt("Iterations", &iterations, 0.1f, 0))
			regenerate = true;

		if (ImGui::DragFloat("Line Thickness", &lineThickness, 0.01f, 0.0f))
		{
			if (lineThickness < 0.0f) lineThickness = 0.0f;
			regenerate = true;
		}

		ImGui::Image((ImTextureID*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	SerializerValue KifsFractalNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetNumber(object, "angle", angle);
		JSONSerializer::SetNumber(object, "reflection", reflectionAngle);
		JSONSerializer::SetNumber(object, "lineThickness", lineThickness);
		JSONSerializer::SetNumber(object, "iterations", iterations);

		float angle = 0.66667f;
		float reflectionAngle = 0.83333f;
		float lineThickness = 1.0f;
		int iterations = 3;

		return value;
	}

	void KifsFractalNode::Load(SerializerObject& obj)
	{
		angle = JSONSerializer::GetNumberFromObject(obj, "angle");
		reflectionAngle = JSONSerializer::GetNumberFromObject(obj, "reflection");
		lineThickness = JSONSerializer::GetNumberFromObject(obj, "lineThickness");
		iterations = JSONSerializer::GetNumberFromObject(obj, "iterations");
	}

}