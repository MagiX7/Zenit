#pragma once

#include "ComputeShaderNode.h"

namespace Zenit {

	// This needs to match with NodeType
	enum class NoiseType
	{
		NORMAL = 1,
		PERLIN,
		DERIVATIVE,
		GRADIENT
	};

	class NoiseNode : public ComputeShaderNode
	{
	public:

		NoiseNode(int id, const char* name, NodeOutputType outputType, NoiseType nType);
		virtual ~NoiseNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		NoiseType noiseType;
		float latestSeed = 0;
		int scale = 10;

		// Perlin
		int numOctaves = 5;

		// FBM
		float gValue = 0.707;
	};

}
