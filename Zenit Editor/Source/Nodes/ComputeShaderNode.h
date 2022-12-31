#pragma once

#include "Node.h"

namespace Zenit {

	class ComputeShaderNode : public Node
	{
	public:
		ComputeShaderNode(int id, const char* name, NodeOutputType outputType);
		virtual ~ComputeShaderNode() {};
		
		void BindCoreData() const;
		void DispatchCompute(int xPixels, int yPixels) const;

		virtual void Update(TimeStep ts) override;

		//virtual SerializerValue Save() {}

	public:
		std::shared_ptr<Texture2D> texture;
		std::unique_ptr<ComputeShader> computeShader;

	protected:
		bool regenerate = true;
	};
}