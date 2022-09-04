#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	class ClampNode : public ComputeShaderNode
	{
	public:
		ClampNode(int id, const char* name, NodeOutputType outputType);
		virtual ~ClampNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputColor(glm::vec3 color);
		void SetInputTexture(const std::unique_ptr<Texture2D>& tex);

	public:
		float min, max;
		bool isTex;
		glm::vec3 inputColor;
		std::unique_ptr<Texture2D> inputTexture;
	};

}