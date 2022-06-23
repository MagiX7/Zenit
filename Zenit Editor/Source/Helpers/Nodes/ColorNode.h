#pragma once

#include <glm/vec4.hpp>

#include "Node.h"

namespace Zenit {

	struct ColorNode : public Node
	{
		ColorNode(int id, const char* name, NodeType type, const glm::vec3& color)
			: Node(id, name, type), color(color)
		{
		}

		glm::vec3 color;
	};
}