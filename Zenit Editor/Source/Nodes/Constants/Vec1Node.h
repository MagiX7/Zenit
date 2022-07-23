#pragma once

#include "Nodes/Node.h"

namespace Zenit {

	class Vec1Node : public Node
	{
	public:
		Vec1Node(int id, const char* name, NodeOutputType outputType);
		~Vec1Node();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiRender() override;

	public:
		float value;

	};

}