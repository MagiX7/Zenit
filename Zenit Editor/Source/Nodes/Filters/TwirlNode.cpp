#include "TwirlNode.h"

namespace Zenit {

	TwirlNode::TwirlNode(int id, const char* name, NodeOutputType outputType)
		: ComputeShaderNode(id, name, outputType)
	{
		type = NodeType::TWIRL;

		computeShader = std::make_unique<ComputeShader>("Assets/Shaders/Compute/twirl.shader");

		texture = std::make_unique<Texture2D>(nullptr, 1024, 1024);
		inputTexture = std::make_unique<Texture2D>("Settings/white.png");

		center = { -0.5f,-0.5f };
		radius = 1.0f;
		angle = 3.0f;
	}

	TwirlNode::~TwirlNode()
	{
	}

	void TwirlNode::Update(TimeStep ts)
	{
		BindCoreData();
		
		inputTexture->Bind(1);
		computeShader->SetUniform1i("inputTexture", 1);
		computeShader->SetUniformVec2f("center", center);
		computeShader->SetUniform1f("radius", radius);
		computeShader->SetUniform1f("angle", angle);

		DispatchCompute(1, 1);
	}

	void TwirlNode::OnImGuiNodeRender()
	{
		ImGui::Image((ImTextureID*)texture->GetId(), { 50,50 }, { 0,1 }, { 1,0 });

	}

	void TwirlNode::OnImGuiInspectorRender()
	{
		ImGui::DragFloat2("Center", glm::value_ptr(center), 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Radius", &radius, 0.01f, 0.0f);
		ImGui::DragFloat("Angle", &angle, 0.01f);

		ImGui::Separator();
		ImGui::Image((void*)texture->GetId(), { 256,256 }, { 0,1 }, { 1,0 });
	}

}