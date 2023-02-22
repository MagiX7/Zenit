#include "TilingNode.h"

namespace Zenit {

	TilingNode::TilingNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType)
	{
		type = NodeType::TILING;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Filters/tiling.shader");
		texture = std::make_shared<Texture2D>(nullptr, 512, 512);
		inputTexture = std::make_unique<Texture2D>("Settings/white.png");
	}

	TilingNode::~TilingNode()
	{
	}

	void TilingNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;


		BindCoreData();

		inputTexture->Bind(1);
		computeShader->SetUniform1i("inputTexture", 1);
		//computeShader->SetUniform1f("horizontalOffset", horizontalOffset);
		//computeShader->SetUniform1f("verticalOffset", verticalOffset);
		computeShader->SetUniformVec2f("offset", { horizontalOffset , verticalOffset });
		computeShader->SetUniform1f("tileNum", tileNum);

		DispatchCompute(1, 1);

		regenerate = false;

	}

	void TilingNode::OnImGuiNodeRender()
	{
		ImGui::Image((void*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void TilingNode::OnImGuiInspectorRender()
	{
		if (ImGui::DragFloat("Horizontal Offset", &horizontalOffset, 0.05f))
			regenerate = true;

		if (ImGui::DragFloat("Vertical Offset", &verticalOffset, 0.05f))
			regenerate = true;

		if (ImGui::DragInt("Number of Tiles", &tileNum))
			regenerate = true;

		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	void TilingNode::SetTexture(Texture2D* texture)
	{
		*inputTexture = *texture;
		regenerate = true;
	}

	SerializerValue TilingNode::Save()
	{
		return SerializerValue();
	}

	void TilingNode::Load(SerializerObject& obj)
	{
	}

}