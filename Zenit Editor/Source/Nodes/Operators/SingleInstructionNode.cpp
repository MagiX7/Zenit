#include "SingleInstructionNode.h"

namespace Zenit {

	SingleInstructionNode::SingleInstructionNode(int id, const char* name, NodeOutputType outputType, SingleInstructionType instructionType)
		: ComputeShaderNode(id, name, outputType)
	{
		this->type = (NodeType)instructionType;
		instrType = instructionType;

		switch (instrType)
		{
			case SingleInstructionType::POW:
			{
				computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Operators/pow.shader");
				break;
			}
		}

		texture = std::make_shared<Texture2D>(nullptr, 512, 512);

		inputTexture = GetWhite();
	}

	SingleInstructionNode::~SingleInstructionNode()
	{
	}

	void SingleInstructionNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		BindCoreData();

		inputTexture->Bind(1);
		computeShader->SetUniform1i("inputTexture", 1);

		switch (instrType)
		{
			case SingleInstructionType::POW:
			{
				computeShader->SetUniform1f("factor", factor);
				break;
			}
		}

		DispatchCompute(1, 1);

		regenerate = false;
	}

	void SingleInstructionNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void SingleInstructionNode::OnImGuiInspectorRender()
	{
		switch (instrType)
		{
			case SingleInstructionType::POW:
			{
				if (ImGui::DragFloat("Factor", &factor, 0.01f, 0.0f))
				{
					if (factor < 0.0f) factor = 0.0f;
					regenerate = true;
				}
				break;
			}
		}		

		ImGui::Image((ImTextureID*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	void SingleInstructionNode::SetInputTexture(Texture2D* texture)
	{
		inputTexture = texture;
		regenerate = true;
	}

	SerializerValue SingleInstructionNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		
		switch (instrType)
		{
			case SingleInstructionType::POW:
			{
				JSONSerializer::SetNumber(object, "factor", factor);
				break;
			}
		}

		return value;
	}

	void SingleInstructionNode::Load(SerializerObject& obj)
	{
		switch (instrType)
		{
			case SingleInstructionType::POW:
			{
				factor = JSONSerializer::GetNumberFromObject(obj, "factor");
				break;
			}
		}

	}

}