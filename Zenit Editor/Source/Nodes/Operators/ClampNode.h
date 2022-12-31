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

		void SetInputTexture(const std::unique_ptr<Texture2D>& tex);
		void SetInputTexture(Texture2D* tex);

		virtual SerializerValue Save() override;

	public:
		float min, max;
		std::unique_ptr<Texture2D> inputTexture;
	};

}