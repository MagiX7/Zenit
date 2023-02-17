#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	class CheckersNode : public ComputeShaderNode
	{
	public:
		CheckersNode(int id, const char* name, NodeOutputType outputType);
		virtual ~CheckersNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		int numCells = 2;

	};

}