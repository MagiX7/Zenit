#pragma once

#include "Node.h"

namespace Zenit {

	struct ComputeShaderNode : public Node
	{
		ComputeShaderNode(int id, const char* name, NodeType type)
			: Node(id, name, type)
		{
		}

		void BindCoreData() const
		{
			texture->BindImage();
			computeShader->Bind();
			computeShader->SetUniform1i("imgOutput", 0);
		}

		void DispatchCompute(int xPixels, int yPixels) const
		{
			glDispatchCompute(texture->GetWidth() / xPixels, texture->GetHeight() / yPixels, 1); // 8 and 4 is bc of the shader file
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}


		std::unique_ptr<Texture2D> texture;
		std::unique_ptr<ComputeShader> computeShader;
	};
}