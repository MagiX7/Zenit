#include "ClampNode.h"

namespace Zenit {

	// TODO: Handle different blend curves (easings maybe?)
	ClampNode::ClampNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType), min(0.0f), max(1.0f)
	{
		type = NodeType::CLAMP;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Operators/clamp.shader");
		texture = std::make_unique<Texture2D>(nullptr, 1024, 1024);

		inputTexture = std::make_unique<Texture2D>("Settings/white.png");

		BindCoreData();
		DispatchCompute(1, 1);
	}

	ClampNode::~ClampNode()
	{
	}

	void ClampNode::Update(TimeStep ts)
	{
		BindCoreData();
		inputTexture->Bind(1);
		computeShader->SetUniform1i("tex", 1);
		
		computeShader->SetUniform1f("min", min);
		computeShader->SetUniform1f("max", max);

		DispatchCompute(1, 1);
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
		ImGui::Image((void*)texture->GetId(), { 512,512 }, { 0,1 }, { 1,0 });
		ImGui::Image((void*)inputTexture->GetId(), { 32,32 }, { 0,1 }, { 1,0 });
	}

	void ClampNode::SetInputTexture(const std::unique_ptr<Texture2D>& tex)
	{
		*inputTexture = *tex;
	}
}