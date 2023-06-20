#pragma once

#include "../Node.h"

namespace Zenit {

	class MaxMinNode : public Node
	{
	public:
		MaxMinNode(int id, const char* name , bool isMax);
		virtual ~MaxMinNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		//inline void SetInputTexture(std::shared_ptr<Texture2D> texture) { inputTexture = texture; }
		//inline std::shared_ptr<Texture2D> GetInputTexture() { return inputTexture; }

		void SetFirstTexture(Texture2D* texture);
		void SetSecondTexture(Texture2D* texture);
	
		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;

	private:
		Texture2D* inputTexture1;
		Texture2D* inputTexture2;
		//enum class Type { MIN, MAX };
		//Type maxMinType;
	};
}