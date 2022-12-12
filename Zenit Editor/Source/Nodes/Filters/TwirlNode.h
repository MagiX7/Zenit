#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	class TwirlNode : public ComputeShaderNode
	{
	public:
		TwirlNode(int id, const char* name, NodeOutputType outputType);
		virtual ~TwirlNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

	public:
		std::unique_ptr<Texture2D> inputTexture;

	private:
		glm::vec2 center;
		float radius;
		float angle;

	};

}