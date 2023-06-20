#include "NoiseNode.h"

namespace Zenit {

	NoiseNode::NoiseNode(int id, const char* name, NoiseType nType)
		: Node(id, name), noiseType(nType)
	{
		//type = NodeType::PERLIN_NOISE;
		//noiseType = NodeType(noiseType);

		switch (noiseType)
		{
			case NoiseType::WHITE:
			{
				computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Generators/white_noise.shader");
				type = NodeType::WHITE_NOISE;
				break;
			}
			case NoiseType::FBM:
			{
				computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Generators/perlin_noise.shader");
				type = NodeType::FBM_NOISE;
				break;
			}
			case NoiseType::DERIVATIVE:
			{
				type = NodeType::DERIVATIVE_NOISE;
				break;
			}
			case NoiseType::GRADIENT:
			{
				computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/Generators/gradient_noise.shader");
				type = NodeType::GRADIENT_NOISE;
				latestSeed = 4.122871f;
				regenerate = true;
				break;
			}
		}

		texture = std::make_shared<Texture2D>(nullptr, NODE_TEXTURE_SIZE, NODE_TEXTURE_SIZE);

		BindCoreData();
		computeShader->SetUniformVec3f("inputColor", { 1,1,1 });
		DispatchCompute(1, 1);
		
	}

	NoiseNode::~NoiseNode()
	{
	}

	void NoiseNode::Update(TimeStep ts)
	{
		if (!regenerate)
			return;

		latestSeed += Application::GetInstance().GetTotalExecutionTime();

		BindCoreData();
		computeShader->SetUniform1f("seed", latestSeed);
		computeShader->SetUniform1i("scale", scale);
		switch (noiseType)
		{
			case NoiseType::FBM:
			{
				computeShader->SetUniform1i("numOctaves", numOctaves);
				break;
			}
			case NoiseType::WHITE:
			{
				break;
			}
			case NoiseType::GRADIENT:
			{
				break;
			}
		}

		DispatchCompute(1, 1);

		Node::Update(ts);

		regenerate = false;
	}

	void NoiseNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void NoiseNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		ImGui::Dummy({ 0, 10 });
		
		if (ImGui::Button("Regenerate"))
			regenerate = true;

		if (ImGui::DragInt("Scale", &scale, 0.1f))
			regenerate = true;

		if (noiseType == NoiseType::FBM)
		{
			if (ImGui::DragInt("Number of Ocatves", &numOctaves, 0.1f, 0))
				regenerate = true;
		}		


		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

	SerializerValue NoiseNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);

		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);
		JSONSerializer::SetNumber(object, "noiseType", (int)noiseType);

		if (noiseType == NoiseType::FBM)
			JSONSerializer::SetNumber(object, "scale", scale);

		return value;
	}

	void NoiseNode::Load(SerializerObject& obj)
	{
		noiseType = (NoiseType)JSONSerializer::GetNumberFromObject(obj, "noiseType");
		if (noiseType == NoiseType::FBM)
			scale = JSONSerializer::GetNumberFromObject(obj, "scale");
	}
}
