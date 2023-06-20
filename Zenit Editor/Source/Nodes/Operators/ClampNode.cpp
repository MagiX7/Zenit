#include "ClampNode.h"

namespace Zenit {

	// TODO: Handle different blend curves (easings maybe?)
	ClampNode::ClampNode(int id, const char* name)
		: Node(id, name), min(0.0f), max(1.0f)
	{
		type = NodeType::CLAMP;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Operators/clamp.shader");
		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);

		inputTexture = GetWhite();

		BindCoreData();
		DispatchCompute(1, 1);
	}

	ClampNode::~ClampNode()
	{
	}

	void ClampNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();
		inputTexture->Bind(1);
		computeShader->SetUniform1i("tex", 1);
		
		computeShader->SetUniform1f("min", min);
		computeShader->SetUniform1f("max", max);

		DispatchCompute(1, 1);

		Node::Update(ts);

		regenerate = false;
	}

	void ClampNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void ClampNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		ImGui::DragFloat("Minimum", &min, 0.01f, 0.0f, max);
		ImGui::DragFloat("Maximum", &max, 0.01f, min, 1.0f);
		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	void ClampNode::SetInputTexture(Texture2D* tex)
	{
		inputTexture = tex;
	}

	SerializerValue ClampNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		Node::SaveCore(object);
		JSONSerializer::SetNumber(object, "max", max);
		JSONSerializer::SetNumber(object, "min", min);

		return value;
	}

	void ClampNode::Load(SerializerObject& obj)
	{
		max = JSONSerializer::GetNumberFromObject(obj, "max");
		min = JSONSerializer::GetNumberFromObject(obj, "min");
	}
}