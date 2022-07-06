#pragma once

#include "ComputeShaderNode.h"

namespace Zenit {

	class VoronoiNode : public ComputeShaderNode
	{
	public:
		VoronoiNode(int id, const char* name, NodeOutputType outputType);
		virtual ~VoronoiNode();

		void Update(TimeStep ts) override;
		void OnImGuiRender() override;

	};

}