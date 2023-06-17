#include "TwirlNode.h"

namespace Zenit {

	TwirlNode::TwirlNode(int id, const char* name)
		: Node(id, name)
	{
		type = NodeType::TWIRL;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Filters/twirl.shader");

		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);
		inputTexture = GetWhite();
		
		center = { -0.5f,-0.5f };
		radius = 1.0f;
		angle = 3.0f;
	}

	TwirlNode::~TwirlNode()
	{
	}

	void TwirlNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();
		
		inputTexture->Bind(1);
		computeShader->SetUniform1i("inputTexture", 1);
		computeShader->SetUniformVec2f("center", center);
		computeShader->SetUniform1f("radius", radius);
		computeShader->SetUniform1f("angle", angle);

		DispatchCompute(1, 1);

		Node::Update(ts);

		regenerate = false;
	}

	void TwirlNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void TwirlNode::OnImGuiInspectorRender()
	{
		bool changedCenter = ImGui::DragFloat2("Center", glm::value_ptr(center), 0.01f, -1.0f, 1.0f);
		bool changedRadius = ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f);
		bool changedAngle = ImGui::DragFloat("Angle", &angle, 0.01f);

		if (changedCenter || changedRadius || changedAngle)
			regenerate = true;

		ImGui::Separator();
		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	void TwirlNode::SetInputTexture(Texture2D* texture)
	{
		inputTexture = texture;
		regenerate = true;
	}

	SerializerValue TwirlNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetVector2f(object, "center", center);
		JSONSerializer::SetNumber(object, "radius", radius);
		JSONSerializer::SetNumber(object, "angle", angle);

		return value;
	}

	void TwirlNode::Load(SerializerObject& obj)
	{
		center = JSONSerializer::GetVector2fFromObject(obj, "center");
		radius = JSONSerializer::GetNumberFromObject(obj, "radius");
		angle = JSONSerializer::GetNumberFromObject(obj, "angle");
	}

}