#include "CheckersNode.h"

namespace Zenit {

	CheckersNode::CheckersNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType)
	{
		type = NodeType::CIRCLE;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Generators/checkers.shader");
		texture = std::make_shared<Texture2D>(nullptr, 512, 512);
	}

	CheckersNode::~CheckersNode()
	{
	}

	void CheckersNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();
		computeShader->SetUniform1i("cells", numCells);
		DispatchCompute(1, 1);
	}

	void CheckersNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void CheckersNode::OnImGuiInspectorRender()
	{
		if (ImGui::DragInt("Number of Cells", &numCells, 2.0f, 0, 50))
			regenerate = true;

		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	SerializerValue CheckersNode::Save()
	{
		return SerializerValue();
	}

	void CheckersNode::Load(SerializerObject& obj)
	{
	}

}
