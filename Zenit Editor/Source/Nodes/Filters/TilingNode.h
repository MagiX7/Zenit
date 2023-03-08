#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	class TilingNode : public ComputeShaderNode
	{
	public:
		TilingNode(int id, const char* name, NodeOutputType outputType);
		virtual ~TilingNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(Texture2D* texture);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		float horizontalOffset = 0.5f;
		float verticalOffset = 0.5f;
		int tileNum = 10;
		Texture2D* inputTexture;

	};


}