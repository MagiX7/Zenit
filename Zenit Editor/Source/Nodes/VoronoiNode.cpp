#include "VoronoiNode.h"

namespace Zenit {

	VoronoiNode::VoronoiNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType), brightness(1.0f)
	{
		type = NodeType::VORONOI;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/voronoi.shader");
		texture = std::make_unique<Texture2D>(nullptr, 512, 512);

		BindCoreData();
		//computeShader->SetUniformVec3f("inputColor", { 1,1,1 });
		DispatchCompute(1, 1);
	}

	VoronoiNode::~VoronoiNode()
	{
	}

	void VoronoiNode::Update(TimeStep ts)
	{
		BindCoreData();
		computeShader->SetUniform1f("brightness", brightness);
		if (regenerate)
		{
			computeShader->SetUniform1f("seed", ts);
			regenerate = false;
		}
		DispatchCompute(1, 1);
	}

	void VoronoiNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void VoronoiNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		ImGui::Dummy({ 0, 10 });

		if (ImGui::Button("Regenerate"))
			regenerate = true;

		ImGui::DragFloat("Brightness", &brightness, 0.1f, 0.0f);
		ImGui::Image((void*)texture->GetId(), { 512,512 }, { 0, 1 }, { 1,0 });
	}
}
