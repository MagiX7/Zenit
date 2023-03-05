#pragma once

#include "ComputeShaderNode.h"

namespace Zenit {

	class GradientNode : public ComputeShaderNode
	{
	public:
		GradientNode(int id, const char* name, NodeOutputType outputType);
		virtual ~GradientNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(Texture2D* tex);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		Texture2D* inputTexture;
		bool vertical = true;
		bool horizontal = false;
		float distance = 1.0f;
	};


}