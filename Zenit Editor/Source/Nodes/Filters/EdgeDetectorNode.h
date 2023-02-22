#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	class EdgeDetectorNode : public ComputeShaderNode
	{
	public:
		EdgeDetectorNode(int id, const char* name, NodeOutputType outputType);
		virtual ~EdgeDetectorNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(std::shared_ptr<Texture2D> white);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		std::shared_ptr<Texture2D> inputTexture;
		int threshold = 1;
	};


}