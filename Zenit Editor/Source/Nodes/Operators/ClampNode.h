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

		void SetInputTexture(Texture2D* tex);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	public:
		float min, max;
		Texture2D* inputTexture;
	};

}