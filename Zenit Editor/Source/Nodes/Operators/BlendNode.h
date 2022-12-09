#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	class BlendNode : public ComputeShaderNode
	{
	public:
		BlendNode(int id, const char* name, NodeOutputType outputType);
		virtual ~BlendNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

	public:
		float contribution;
		std::unique_ptr<Texture2D> tex1;
		std::unique_ptr<Texture2D> tex2;

		enum class BlendMode
		{
			LINEAR, BURN, DARKEN, DIFFERENCE, DODGE, DIVIDE, MULTIPLY, NEGATION, SUBTRACT
		} blendMode;

		int comboCurrentIndex = 0;

	};

}