#pragma once

#include "Node.h"

namespace Zenit {

	class TransformNode : public Node
	{
	public:
		TransformNode(int id, const char* name);
		virtual ~TransformNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(Texture2D* texture);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		Texture2D* inputTexture;
		
		glm::vec2 offset = { 0,0 };
		float rotationAngle = 0.0f;
		int scale = 1;

		glm::mat3 transform = glm::mat3(1.0);
	};

}
