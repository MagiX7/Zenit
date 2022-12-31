#pragma once

#include "ComputeShaderNode.h"

namespace Zenit {

	class VoronoiNode : public ComputeShaderNode
	{
	public:
		VoronoiNode(int id, const char* name, NodeOutputType outputType);
		virtual ~VoronoiNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		virtual SerializerValue Save() override;

	public:
		float brightness;
		float latestSeed;
		float zoom;
	};

}