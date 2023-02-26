#include "EdgeDetectorNode.h"

namespace Zenit {

	EdgeDetectorNode::EdgeDetectorNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType)
	{
		type = NodeType::EDGE_DETECTOR;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Filters/edge_detector.shader");
		texture = std::make_shared<Texture2D>(nullptr, 512, 512);
		inputTexture = GetWhite();
	}

	EdgeDetectorNode::~EdgeDetectorNode()
	{
	}

	void EdgeDetectorNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();

		inputTexture->Bind(1);
		computeShader->SetUniform1i("inputTexture", 1);
		computeShader->SetUniform1i("threshold", threshold);

		DispatchCompute(1, 1);
		
		regenerate = false;
	}

	void EdgeDetectorNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void EdgeDetectorNode::OnImGuiInspectorRender()
	{
		if (ImGui::DragInt("Threshold", &threshold, 0.05f))
			regenerate = true;

		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	void EdgeDetectorNode::SetInputTexture(Texture2D* texture)
	{
		inputTexture = texture;
		regenerate = true;
	}

	SerializerValue EdgeDetectorNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetNumber(object, "threshold", threshold);

		return value;
	}

	void EdgeDetectorNode::Load(SerializerObject& obj)
	{
		JSONSerializer::GetNumberFromObject(obj, "threshold");
	}

}