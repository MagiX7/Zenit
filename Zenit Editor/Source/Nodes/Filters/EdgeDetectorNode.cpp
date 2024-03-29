#include "EdgeDetectorNode.h"

namespace Zenit {

	EdgeDetectorNode::EdgeDetectorNode(int id, const char* name)
		: Node(id, name)
	{
		type = NodeType::EDGE_DETECTOR;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Filters/edge_detector.shader");
		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);
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
		
		Node::Update(ts);

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

		Node::SaveCore(object);
		JSONSerializer::SetNumber(object, "threshold", threshold);

		return value;
	}

	void EdgeDetectorNode::Load(SerializerObject& obj)
	{
		threshold = JSONSerializer::GetNumberFromObject(obj, "threshold");
	}

}