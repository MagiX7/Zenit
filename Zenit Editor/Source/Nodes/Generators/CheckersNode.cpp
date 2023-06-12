#include "CheckersNode.h"

namespace Zenit {

	CheckersNode::CheckersNode(int id, const char* name)
		: Node(id, name)
	{
		type = NodeType::CHECKERS;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Generators/checkers.shader");
		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);
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

		regenerate = false;
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
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetNumber(object, "numCells", numCells);

		return value;
	}

	void CheckersNode::Load(SerializerObject& obj)
	{
		numCells= JSONSerializer::GetNumberFromObject(obj, "numCells");
	}

}
