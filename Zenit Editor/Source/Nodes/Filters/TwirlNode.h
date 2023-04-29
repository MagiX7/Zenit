#pragma once

#include "../Node.h"

namespace Zenit {

	class TwirlNode : public Node
	{
	public:
		TwirlNode(int id, const char* name, NodeOutputType outputType);
		virtual ~TwirlNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(Texture2D* texture);
		
		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		Texture2D* inputTexture;
		glm::vec2 center;
		float radius;
		float angle;

	};

}