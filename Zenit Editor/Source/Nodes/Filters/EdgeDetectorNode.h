#pragma once

#include "../Node.h"

namespace Zenit {

	class EdgeDetectorNode : public Node
	{
	public:
		EdgeDetectorNode(int id, const char* name);
		virtual ~EdgeDetectorNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		void SetInputTexture(Texture2D* white);

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		Texture2D* inputTexture;
		int threshold = 20;
	};


}