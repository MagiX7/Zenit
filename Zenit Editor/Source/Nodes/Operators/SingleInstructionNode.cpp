#include "SingleInstructionNode.h"

namespace Zenit {

	SingleInstructionNode::SingleInstructionNode(int id, const char* name, NodeOutputType outputType, SingleInstructionType instructionType)
		: Node(id, name, outputType)
	{
		this->type = (NodeType)instructionType;
		instrType = instructionType;

		switch (instrType)
		{
			case SingleInstructionType::POW:
			{
				computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Operators/pow.shader");
				factor = 1.0f;
				break;
			}
			case SingleInstructionType::ADD:
			{
				computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Operators/add.shader");
				factor = 0.0f;
				color = glm::vec3(1);
				break;
			}
			case SingleInstructionType::SUBSTRACT:
			{
				computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Operators/substract.shader");
				factor = 0.0f;
				color = glm::vec3(1);
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

		computeShader->SetUniform1f("factor", factor);

	
		switch (instrType)
		{
			case SingleInstructionType::ADD:
			case SingleInstructionType::SUBSTRACT:
			{
				computeShader->SetUniformVec3f("inColor", color);
				computeShader->SetUniform1i("useColor", useColor);
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
			case SingleInstructionType::ADD:
			case SingleInstructionType::SUBSTRACT:
			{
				if (ImGui::DragFloat("Factor", &factor, 0.01f, 0.0f, 1.0f))
				{
					if (factor < 0.0f) factor = 0.0f;
					regenerate = true;
				}

				ImGui::Checkbox("Use color", &useColor);

				if (useColor)
				{
					if (ImGui::CollapsingHeader("Color", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::SetNextItemWidth(128);
						if (ImGui::ColorPicker3("Color", glm::value_ptr(color)))
							regenerate = true;
					}
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
		
		JSONSerializer::SetNumber(object, "factor", factor);
		
		switch (instrType)
		{
			case SingleInstructionType::ADD:
			case SingleInstructionType::SUBSTRACT:
			{
				JSONSerializer::SetVector3f(object, "color", color);
				JSONSerializer::SetNumber(object, "useColor", useColor);
				break;
			}
		}

		return value;
	}

	void SingleInstructionNode::Load(SerializerObject& obj)
	{
		factor = JSONSerializer::GetNumberFromObject(obj, "factor");
		//switch (instrType)
		//{
		//	case SingleInstructionType::POW:
		//	case SingleInstructionType::ADD:
		//	case SingleInstructionType::SUBSTRACT:
		//	{
		//		break;
		//	}
		//}
	}

}