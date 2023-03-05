#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	class KifsFractalNode : public ComputeShaderNode
	{
	public:
		KifsFractalNode(int id, const char* name, NodeOutputType outputType);
		virtual ~KifsFractalNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		float angle = 0.66667f;
		float reflectionAngle = 0.83333f;
		float lineThickness = 1.0f;
		int iterations = 3;
	};

}
