#include "VoronoiNode.h"

namespace Zenit {

	VoronoiNode::VoronoiNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType)
	{
		type = NodeType::VORONOI;
	}

	VoronoiNode::~VoronoiNode()
	{
	}

	void VoronoiNode::Update(TimeStep ts)
	{
		BindCoreData();
		computeShader->SetUniform1f("time", ts);
		DispatchCompute(8, 4);
	}

	void VoronoiNode::OnImGuiRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}
}
