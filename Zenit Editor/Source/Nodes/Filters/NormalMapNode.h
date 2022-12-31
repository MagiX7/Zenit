#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	class NormalMapNode : public ComputeShaderNode
	{
	public:
		NormalMapNode(int id, const char* name, NodeOutputType outputType);
		virtual ~NormalMapNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetTexture(std::shared_ptr<Texture2D> texture);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		std::shared_ptr<Texture2D> inputTexture;
		float zoom = 1.0f;
		float bumpness = 1.0f;
	};

}