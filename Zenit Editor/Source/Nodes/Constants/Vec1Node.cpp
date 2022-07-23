#include "Vec1Node.h"

namespace Zenit {

	Vec1Node::Vec1Node(int id, const char* name, NodeOutputType outputType)
		: Node(id, name, outputType), value(1.0f)
	{
	}

	Vec1Node::~Vec1Node()
	{
	}

	void Vec1Node::Update(TimeStep ts)
	{
	}

	void Vec1Node::OnImGuiRender()
	{
		ImGui::SetNextItemWidth(50);
		ImGui::DragFloat("##Value", &value, 0.01f, 0.0f);
	}

}