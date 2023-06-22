#include "InvertNode.h"

namespace Zenit {

	InvertNode::InvertNode(int id, const char* name)
		: Node(id, name)
	{
		type = NodeType::INVERT;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Filters/invert.shader");
		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);
		inputTexture = GetWhite();
	}

	InvertNode::~InvertNode()
	{
	}

	void InvertNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();
		inputTexture->Bind(1);
		computeShader->SetUniform1i("inputTexture", 1);
		DispatchCompute(1, 1);
		
		Node::Update(ts);
		
		regenerate = false;
	}

	void InvertNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void InvertNode::OnImGuiInspectorRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	void InvertNode::SetInputTexture(Texture2D* tex)
	{
		inputTexture = tex;
		regenerate = true;
	}

	SerializerValue InvertNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);
		Node::SaveCore(object);		
		return value;
	}

	void InvertNode::Load(SerializerObject& obj)
	{
	}

}