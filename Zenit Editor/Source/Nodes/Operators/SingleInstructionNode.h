#pragma once

#include "../ComputeShaderNode.h"

namespace Zenit {

	enum class SingleInstructionType
	{
		POW = 19,
	};

	class SingleInstructionNode : public ComputeShaderNode
	{
	public:
		SingleInstructionNode(int id, const char* name, NodeOutputType outputType, SingleInstructionType type);
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
		float factor = 1.0f;
	};

}