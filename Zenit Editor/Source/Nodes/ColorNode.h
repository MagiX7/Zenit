#pragma once

#include <glm/vec4.hpp>

#include "Node.h"

namespace Zenit {

	class ColorNode : public Node
	{
	public:
		ColorNode(int id, const char* name, NodeOutputType outputType, const glm::vec3& color);

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

	public:
		glm::vec3 color;
	};
}