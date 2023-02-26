#include "ComputeShaderNode.h"

namespace Zenit {

	//std::shared_ptr<Texture2D> ComputeShaderNode::white = std::make_shared<Texture2D>("Settings/white.png");
	Texture2D* ComputeShaderNode::white = nullptr;


	ComputeShaderNode::ComputeShaderNode(int id, const char* name, NodeOutputType outputType)
		: Node(id, name, outputType)
	{
	}

	void ComputeShaderNode::BindCoreData() const
	{
		texture->BindImage();
		computeShader->Bind();
		computeShader->SetUniform1i("imgOutput", 0);
		//texture->UnbindImage();
	}

	void ComputeShaderNode::DispatchCompute(int xPixels, int yPixels) const
	{
		glDispatchCompute(texture->GetWidth() / xPixels, texture->GetHeight() / yPixels, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		computeShader->Unbind();
	}

	void ComputeShaderNode::Update(TimeStep ts)
	{
	}

	Texture2D* ComputeShaderNode::GetWhite()
	{
		if (!white)
			white = new Texture2D("Settings/white.png");
		return white;
	}



}
