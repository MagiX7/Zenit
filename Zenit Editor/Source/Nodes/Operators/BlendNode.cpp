#include "BlendNode.h"

namespace Zenit {

	BlendNode::BlendNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType), contribution(0)
	{
		type = NodeType::BLEND;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Operators/blend.shader");
		texture = std::make_shared<Texture2D>(nullptr, 1024, 1024);

		BindCoreData();
		computeShader->SetUniform1f("contribution", contribution);
		DispatchCompute(1, 1);

		tex1 = std::make_unique<Texture2D>("Settings/white.png");
		tex2 = std::make_unique<Texture2D>("Settings/white.png");
		tex1->SetName(name + '_' + id);
		tex2->SetName(name + '_' + id);


		blendMode = (BlendMode)0;
	}

	BlendNode::~BlendNode()
	{
	}

	void BlendNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();

		computeShader->SetUniform1f("contribution", contribution);
		computeShader->SetUniform1i("mode", (int)blendMode);
		tex1->Bind(1);
		computeShader->SetUniform1i("tex1", 1);
		tex2->Bind(2);
		computeShader->SetUniform1i("tex2", 2);

		DispatchCompute(1, 1);
	}

	void BlendNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void BlendNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		ImGui::DragFloat("Distribution", &contribution, 0.01f, 0.0f, 1.0f);

		const char* labels[] = { "Burn", "Darken", "Difference", "Dodge", "Divide", "Multiply", "Negation", "Subtract"};
		const char* previewValue = labels[comboCurrentIndex];
		if (ImGui::BeginCombo("Mode", previewValue))
		{
			for (int i = 0; i < IM_ARRAYSIZE(labels); ++i)
			{
				const bool isSelected = (comboCurrentIndex == i);
				if (ImGui::Selectable(labels[i], isSelected))
				{
					blendMode = (BlendMode)i;
					regenerate = true;
					comboCurrentIndex = i;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();

		}

		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	void BlendNode::SetFirstTexture(Texture2D* texture)
	{
		tex1.reset(texture);
		regenerate = true;
	}

	void BlendNode::SetSecondTexture(Texture2D* texture)
	{
		tex2.reset(texture);
		regenerate = true;
	}

	SerializerValue BlendNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetString(object, "firstTexture", tex1->GetName().c_str());
		JSONSerializer::SetString(object, "secondTexture", tex2->GetName().c_str());
		JSONSerializer::SetNumber(object, "blendMode", (int)blendMode);
		JSONSerializer::SetNumber(object, "contribution", contribution);

		return value;
	}

	void BlendNode::Load(SerializerObject& obj)
	{
		blendMode = (BlendMode)JSONSerializer::GetNumberFromObject(obj, "blendMode");
		contribution = JSONSerializer::GetNumberFromObject(obj, "contribution");	
	}

}