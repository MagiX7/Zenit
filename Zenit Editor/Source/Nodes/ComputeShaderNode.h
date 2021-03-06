#pragma once

#include "Node.h"

namespace Zenit {

	class ComputeShaderNode : public Node
	{
	public:

		ComputeShaderNode(int id, const char* name, NodeOutputType outputType);
		
		void BindCoreData() const;

		void DispatchCompute(int xPixels, int yPixels) const;

		virtual void Update(TimeStep ts) override;

	public:
		std::unique_ptr<Texture2D> texture;
		std::unique_ptr<ComputeShader> computeShader;
	};
}