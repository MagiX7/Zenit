#pragma once

#include "../Node.h"

namespace Zenit {

	class CircleNode : public Node
	{
	public:
		CircleNode(int id, const char* name, NodeOutputType outputType);
		virtual ~CircleNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;
		
		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		glm::vec2 offset;
		float radius;
		float blur;
		//float size;
	};

}