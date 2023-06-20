#pragma once

#include "../Node.h"

namespace Zenit {

	class BlendNode : public Node
	{
	public:
		BlendNode(int id, const char* name);
		virtual ~BlendNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetFirstTexture(Texture2D* texture);
		void SetSecondTexture(Texture2D* texture);
		
		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		float contribution;
		Texture2D* tex1;
		Texture2D* tex2;

		enum class BlendMode
		{
			BURN, DARKEN, DIFFERENCE, DODGE, DIVIDE, MULTIPLY, NEGATION, SUBTRACT
		} blendMode;

		int comboCurrentIndex = 0;

	};

}