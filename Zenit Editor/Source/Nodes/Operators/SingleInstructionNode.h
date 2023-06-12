#pragma once

#include "../Node.h"

namespace Zenit {

	enum class SingleInstructionType
	{
		POW = 19,
		ADD = 20,
		SUBTRACT = 21,
	};

	class SingleInstructionNode : public Node
	{
	public:
		SingleInstructionNode(int id, const char* name, SingleInstructionType type);
		virtual ~SingleInstructionNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(Texture2D* texture);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		SingleInstructionType instrType;
		Texture2D* inputTexture;
		float factor = -1.0f;
		bool useColor = false;
		glm::vec3 color = glm::vec3(-1);
	};

}