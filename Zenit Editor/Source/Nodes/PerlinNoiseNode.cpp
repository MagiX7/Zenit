#include "PerlinNoiseNode.h"

namespace Zenit {

	PerlinNoiseNode::PerlinNoiseNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType), seed(1.0f)
	{
		type = NodeType::PERLIN_NOISE;
	}

	PerlinNoiseNode::~PerlinNoiseNode()
	{
	}

	void PerlinNoiseNode::Update(TimeStep ts)
	{
		BindCoreData();
		computeShader->SetUniform1f("seed", seed);
		DispatchCompute(8, 4);
	}

	void PerlinNoiseNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void PerlinNoiseNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		ImGui::DragFloat("Seed", &seed);
		ImGui::Image((void*)texture->GetId(), { 512,512 }, { 0,1 }, { 1,0 });
	}
}
