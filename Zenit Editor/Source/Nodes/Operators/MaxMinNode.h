#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	class MaxMinNode : public ComputeShaderNode
	{
	public:
		MaxMinNode(int id, const char* name, NodeOutputType outputType, bool isMax);
		virtual ~MaxMinNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		//inline void SetInputTexture(std::shared_ptr<Texture2D> texture) { inputTexture = texture; }
		//inline std::shared_ptr<Texture2D> GetInputTexture() { return inputTexture; }

		void SetFirstTexture(Texture2D* texture);
		void SetSecondTexture(Texture2D* texture);
	
		virtual SerializerValue Save() override;

	private:
		std::shared_ptr<Texture2D> inputTexture1;
		std::shared_ptr<Texture2D> inputTexture2;
		//enum class Type { MIN, MAX };
		//Type maxMinType;
	};
}