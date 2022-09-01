#include "NormalMapNode.h"

namespace Zenit {

	NormalMapNode::NormalMapNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType)
	{
		type = NodeType::NORMAL_MAP;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/normal_map.shader");
		texture = std::make_unique<Texture2D>(nullptr, 512, 512);
		inputTexture = std::make_unique<Texture2D>("Settings/white.png");
	}

	NormalMapNode::~NormalMapNode()
	{
	}

	void NormalMapNode::Update(TimeStep ts)
	{
		BindCoreData();
		inputTexture->Bind();
		computeShader->SetUniform1i("inputTexture", 0);
		inputTexture->Unbind();
		DispatchCompute(1, 1);
		//inputTexture->UnbindImage();
	}

	void NormalMapNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void NormalMapNode::OnImGuiInspectorRender()
	{
		ImGui::Image((void*)texture->GetId(), { 512,512 }, { 0,1 }, { 1,0 });
		ImGui::Separator();
		ImGui::Image((ImTextureID*)inputTexture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}
}