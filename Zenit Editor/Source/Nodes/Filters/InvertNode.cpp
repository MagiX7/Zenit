#include "InvertNode.h"

namespace Zenit {

	InvertNode::InvertNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType)
	{
		type = NodeType::INVERT;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Filters/invert.shader");
		texture = std::make_shared<Texture2D>(nullptr, 512, 512);
		inputTexture = std::make_unique<Texture2D>("Settings/white.png");
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

	void InvertNode::SetInputTexture(std::shared_ptr<Texture2D> tex)
	{
		inputTexture = tex;
		regenerate = true;
	}

	SerializerValue InvertNode::Save()
	{
		return SerializerValue();
	}

	void InvertNode::Load(SerializerObject& obj)
	{
	}

}