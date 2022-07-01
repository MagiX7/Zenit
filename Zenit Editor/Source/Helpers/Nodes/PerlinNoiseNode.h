#pragma once

#include "ComputeShaderNode.h"

namespace Zenit {

	class PerlinNoiseNode : public ComputeShaderNode
	{
	public:

		PerlinNoiseNode(int id, const char* name, NodeOutputType outputType);

		void Update(TimeStep ts) override;
		void OnImGuiRender() override;

	public:
		float seed;
	};

}
