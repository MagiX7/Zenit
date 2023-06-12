#pragma once

#include "../Node.h"

namespace Zenit {

	class ClampNode : public Node
	{
	public:
		ClampNode(int id, const char* name);
		virtual ~ClampNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(Texture2D* tex);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	public:
		float min, max;
		Texture2D* inputTexture;
	};

}