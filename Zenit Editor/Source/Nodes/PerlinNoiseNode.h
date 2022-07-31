#pragma once

#include "ComputeShaderNode.h"

namespace Zenit {

	class PerlinNoiseNode : public ComputeShaderNode
	{
	public:

		PerlinNoiseNode(int id, const char* name, NodeOutputType outputType);
		virtual ~PerlinNoiseNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

	public:
		float seed;
	};

}
