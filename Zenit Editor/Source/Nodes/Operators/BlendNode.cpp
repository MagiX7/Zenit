#include "BlendNode.h"

namespace Zenit {

	BlendNode::BlendNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType), contribution(0)
	{
		type = NodeType::BLEND;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Operators/blend.shader");
		texture = std::make_unique<Texture2D>(nullptr, 1024, 1024);

		BindCoreData();
		computeShader->SetUniform1f("contribution", contribution);
		DispatchCompute(1, 1);

		tex1 = std::make_unique<Texture2D>("Settings/white.png");
		tex2 = std::make_unique<Texture2D>("Settings/white.png");
	}

	BlendNode::~BlendNode()
	{
	}

	void BlendNode::Update(TimeStep ts)
	{
		BindCoreData();

		computeShader->SetUniform1f("contribution", contribution);
		tex1->Bind(1);
		computeShader->SetUniform1i("tex1", 1);
		tex2->Bind(2);
		computeShader->SetUniform1i("tex2", 2);

		DispatchCompute(1, 1);
	}

	void BlendNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void BlendNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		ImGui::DragFloat("Distribution", &contribution, 0.01f, 0.0f, 1.0f);
		ImGui::Image((void*)texture->GetId(), { 512,512 }, { 0,1 }, { 1,0 });
	}

}