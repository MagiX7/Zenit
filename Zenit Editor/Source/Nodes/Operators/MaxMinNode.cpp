#include "MaxMinNode.h"

namespace Zenit {

	MaxMinNode::MaxMinNode(int id, const char* name, NodeOutputType outputType, bool isMax)
		: ComputeShaderNode(id, name, outputType)
	{
		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Operators/maxmin.shader");
		texture = std::make_unique<Texture2D>(nullptr, 1024, 1024);

		BindCoreData();
		DispatchCompute(1, 1);

		inputTexture1 = std::make_unique<Texture2D>("Settings/white.png");
		inputTexture2 = std::make_unique<Texture2D>("Settings/white.png");

		maxMinType = (Type)isMax;
		isMax ? type = NodeType::MAX : type = NodeType::MIN;
	}

	MaxMinNode::~MaxMinNode()
	{
	}

	void MaxMinNode::Update(TimeStep ts)
	{
		BindCoreData();
		computeShader->SetUniform1f("type", (int)maxMinType);
		
		inputTexture1->Bind(1);
		computeShader->SetUniform1i("tex1", 1);
		inputTexture2->Bind(2);
		computeShader->SetUniform1i("tex2", 2);

		DispatchCompute(1, 1);
	}

	void MaxMinNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void MaxMinNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();

		ImGui::Image((ImTextureID*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });

	}
}