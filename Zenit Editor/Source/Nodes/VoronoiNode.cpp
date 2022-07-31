#include "VoronoiNode.h"

namespace Zenit {

	VoronoiNode::VoronoiNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType), brightness(1.0f)
	{
		type = NodeType::VORONOI;
	}

	VoronoiNode::~VoronoiNode()
	{
	}

	void VoronoiNode::Update(TimeStep ts)
	{
		BindCoreData();
		computeShader->SetUniform1f("brightness", brightness);
		DispatchCompute(8, 4);
	}

	void VoronoiNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void VoronoiNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		ImGui::DragFloat("Brightness", &brightness, 0.1f, 0.0f);
		ImGui::Image((void*)texture->GetId(), { 512,512 }, { 0, 1 }, { 1,0 });
	}
}
