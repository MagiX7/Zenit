#include "NoiseNode.h"

namespace Zenit {

	NoiseNode::NoiseNode(int id, const char* name, NodeOutputType outputType, NoiseType nType)
		: ComputeShaderNode(id, name, outputType), noiseType(nType)
	{
		//type = NodeType::PERLIN_NOISE;
		type = NodeType(noiseType);

		switch (noiseType)
		{
			case NoiseType::NORMAL:
			{
				computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/noise.shader");
				break;
			}
			case NoiseType::PERLIN:
			{
				computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/perlin_noise.shader");
				break;
			}
		}

		texture = std::make_shared<Texture2D>(nullptr, 512, 512);

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
		regenerate = false;

		switch (noiseType)
		{
			case NoiseType::PERLIN:
			{
				computeShader->SetUniform1i("resolution", res);
				break;
			}
			case NoiseType::NORMAL:
			{
				break;
			}
		}

		DispatchCompute(1, 1);
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

		if (noiseType == NoiseType::PERLIN)
		{
			ImGui::DragInt("Resolution", &res, 1.0f, 0, 10);
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

		if (noiseType == NoiseType::PERLIN)
			JSONSerializer::SetNumber(object, "resolution", res);

		return value;
	}

	void NoiseNode::Load(SerializerObject& obj)
	{
		noiseType = (NoiseType)JSONSerializer::GetNumberFromObject(obj, "noiseType");
		if (noiseType == NoiseType::PERLIN)
			res = JSONSerializer::GetNumberFromObject(obj, "resolution");
	}
}
