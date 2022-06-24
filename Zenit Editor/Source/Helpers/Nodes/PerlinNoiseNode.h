#pragma once

#include "Node.h"

namespace Zenit {

	struct PerlinNoiseNode : public Node
	{
		PerlinNoiseNode(int id, const char* name, NodeType type)
			: Node(id, name, type)
		{
		}

		std::unique_ptr<Texture2D> noiseTex;
		std::unique_ptr<ComputeShader> noiseShader;
	};
}