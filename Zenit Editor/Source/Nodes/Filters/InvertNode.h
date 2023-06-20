#pragma once

#include "../Node.h"

namespace Zenit {

	class InvertNode : public Node
	{
	public:
		InvertNode(int id, const char* name);
		virtual ~InvertNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(Texture2D* tex);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		Texture2D* inputTexture;

	};

}