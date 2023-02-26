#pragma once

#include "ComputeShaderNode.h"

namespace Zenit {

	class TransformNode : public ComputeShaderNode
	{
	public:
		TransformNode(int id, const char* name, NodeOutputType outputType);
		virtual ~TransformNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(std::shared_ptr<Texture2D> texture);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		std::shared_ptr<Texture2D> inputTexture;
		
		glm::vec2 offset = { 0,0 };
		float rotationAngle = 0.0f;
		glm::vec2 scale = { 1,1 };

		glm::mat3 transform = glm::mat3(1.0);
	};

}