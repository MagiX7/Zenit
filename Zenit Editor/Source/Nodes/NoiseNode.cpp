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

		texture = std::make_unique<Texture2D>(nullptr, 1024, 1024);

		BindCoreData();
		computeShader->SetUniformVec3f("inputColor", { 1,1,1 });
		DispatchCompute(8, 4);

	}

	NoiseNode::~NoiseNode()
	{
	}

	void NoiseNode::Update(TimeStep ts)
	{
		BindCoreData();
		if (regenerate)
		{
			computeShader->SetUniform1f("seed", ts);
			regenerate = false;
		}

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

		DispatchCompute(8, 4);
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

		ImGui::Image((void*)texture->GetId(), { 512,512 }, { 0,1 }, { 1,0 });
	}
}
