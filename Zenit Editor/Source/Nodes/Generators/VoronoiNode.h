#pragma once

#include "../Node.h"

namespace Zenit {

	class VoronoiNode : public Node
	{
	public:
		VoronoiNode(int id, const char* name, NodeOutputType outputType);
		virtual ~VoronoiNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	public:
		float latestSeed;
		int scale;
	};

}