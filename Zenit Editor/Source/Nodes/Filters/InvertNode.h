#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	class InvertNode : public ComputeShaderNode
	{
	public:
		InvertNode(int id, const char* name, NodeOutputType outputType);
		virtual ~InvertNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(Texture2D* tex);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		Texture2D* inputTexture;

	};

}