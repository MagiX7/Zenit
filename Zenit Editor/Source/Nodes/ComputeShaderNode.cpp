#include "ComputeShaderNode.h"

namespace Zenit {

	ComputeShaderNode::ComputeShaderNode(int id, const char* name, NodeOutputType outputType)
		: Node(id, name, outputType)
	{
	}

	void ComputeShaderNode::BindCoreData() const
	{
		texture->BindImage();
		computeShader->Bind();
		computeShader->SetUniform1i("imgOutput", 0);
	}

	void ComputeShaderNode::DispatchCompute(int xPixels, int yPixels) const
	{
		glDispatchCompute(texture->GetWidth() / xPixels, texture->GetHeight() / yPixels, 1); // 8 and 4 is bc of the shader file
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	void ComputeShaderNode::Update(TimeStep ts)
	{
	}
}
