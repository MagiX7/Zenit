#include "GradientNode.h"

namespace Zenit {

	GradientNode::GradientNode(int id, const char* name)
		: Node(id, name)
	{
		type = NodeType::GRADIENT;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/gradient.shader");
		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);

		inputTexture = GetWhite();
	}

	GradientNode::~GradientNode()
	{
	}

	void GradientNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;


		BindCoreData();
		inputTexture->Bind(1);
		computeShader->SetUniform1i("inputTexture", 1);

		computeShader->SetUniform1i("vertical", vertical);
		computeShader->SetUniform1i("horizontal", horizontal);
		computeShader->SetUniform1f("distance", distance);
		DispatchCompute(1, 1);

		Node::Update(ts);

		regenerate = false;
	}

	void GradientNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void GradientNode::OnImGuiInspectorRender()
	{
		if (ImGui::Checkbox("Vertical", &vertical))
			regenerate = true;

		if (ImGui::Checkbox("Horizontal", &horizontal))
			regenerate = true;

		if (ImGui::DragFloat("Gradient Distance", &distance, 0.01f))
			regenerate = true;

		ImGui::Image((ImTextureID*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	void GradientNode::SetInputTexture(Texture2D* tex)
	{
		inputTexture = tex;
		regenerate = true;
	}

	SerializerValue GradientNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		Node::SaveCore(object);
		JSONSerializer::SetNumber(object, "vertical", vertical);
		JSONSerializer::SetNumber(object, "horizontal", horizontal);
		JSONSerializer::SetNumber(object, "distance", distance);

		return value;
	}

	void GradientNode::Load(SerializerObject& obj)
	{
		vertical = JSONSerializer::GetNumberFromObject(obj, "vertical");
		horizontal = JSONSerializer::GetNumberFromObject(obj, "horizontal");
		distance = JSONSerializer::GetNumberFromObject(obj, "distance");
	}

}