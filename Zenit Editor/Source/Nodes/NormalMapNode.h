#pragma once

#include "ComputeShaderNode.h"

namespace Zenit {

	class NormalMapNode : public ComputeShaderNode
	{
	public:
		NormalMapNode(int id, const char* name, NodeOutputType outputType);
		virtual ~NormalMapNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

	public:
		std::shared_ptr<Texture2D> inputTexture;
	};

}