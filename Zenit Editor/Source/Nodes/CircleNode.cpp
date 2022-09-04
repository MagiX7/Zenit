#include "CircleNode.h"

namespace Zenit {

	CircleNode::CircleNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType), radius(0.3), blur(0.001)
	{
		offset = { 0,0 };

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/circle.shader");
		texture = std::make_unique<Texture2D>(nullptr, 1024, 1024);

		BindCoreData();
		//computeShader->SetUniform1i("inputTexture", 0);
		DispatchCompute(1, 1);
	}

	CircleNode::~CircleNode()
	{
	}

	void CircleNode::Update(TimeStep ts)
	{
		BindCoreData();
		computeShader->SetUniform1f("radius", radius);
		computeShader->SetUniform1f("blur", blur);
		computeShader->SetUniformVec2f("offset", offset);
		DispatchCompute(1, 1);
	}

	void CircleNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });
	}

	void CircleNode::OnImGuiInspectorRender()
	{
		ImGui::Separator();
		ImGui::DragFloat2("Offset", glm::value_ptr(offset), 0.01);
		ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Blur", &blur, 0.01f, 0.0f, 1.0f);
		ImGui::Image((void*)texture->GetId(), { 512,512 }, { 0,1 }, { 1,0 });
	}
}