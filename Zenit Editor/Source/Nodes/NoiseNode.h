#pragma once

#include "ComputeShaderNode.h"

namespace Zenit {

	// This needs to match with NodeType
	enum class NoiseType
	{
		NORMAL = 1,
		PERLIN,
		DERIVATIVE
	};

	class NoiseNode : public ComputeShaderNode
	{
	public:

		NoiseNode(int id, const char* name, NodeOutputType outputType, NoiseType nType);
		virtual ~NoiseNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

	private:
		NoiseType noiseType;
		int res = 10;
	};

}
