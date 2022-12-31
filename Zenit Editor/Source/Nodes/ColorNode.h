#pragma once

#include <glm/vec4.hpp>

#include "ComputeShaderNode.h"

namespace Zenit {

	class ColorNode : public ComputeShaderNode
	{
	public:
		ColorNode(int id, const char* name, NodeOutputType outputType, const glm::vec3& color);

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	public:
		glm::vec3 color;
	};
}