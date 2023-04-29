#include "TilingNode.h"

namespace Zenit {

	TilingNode::TilingNode(int id, const char* name, NodeOutputType outputType)
		: Node(id, name, outputType)
	{
		type = NodeType::TILING;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Filters/tiling.shader");
		texture = std::make_shared<Texture2D>(nullptr, 512, 512);
		inputTexture = GetWhite();
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

	void TilingNode::SetInputTexture(Texture2D* texture)
	{
		inputTexture = texture;
		regenerate = true;
	}

	SerializerValue TilingNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetNumber(object, "horizontalOffset", horizontalOffset);
		JSONSerializer::SetNumber(object, "verticalOffset", verticalOffset);
		JSONSerializer::SetNumber(object, "tileNum", tileNum);

		return value;
	}

	void TilingNode::Load(SerializerObject& obj)
	{
		horizontalOffset = JSONSerializer::GetNumberFromObject(obj, "horizontalOffset");
		verticalOffset = JSONSerializer::GetNumberFromObject(obj, "verticalOffset");
		tileNum = JSONSerializer::GetNumberFromObject(obj, "tileNum");
	}

}