#pragma once

#include <glm/vec4.hpp>

#include "Node.h"

namespace Zenit {

	class ColorNode : public Node
	{
	public:
		ColorNode(int id, const char* name, NodeOutputType outputType, const glm::vec3& color);

		virtual void Update(TimeStep) override;

		virtual void OnImGuiRender() override;

	public:
		glm::vec3 color;
	};
}