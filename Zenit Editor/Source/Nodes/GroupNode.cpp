#include "GroupNode.h"

namespace Zenit {

	GroupNode::GroupNode(int id, const char* name)
		: Node(id, name)
	{

	}

	GroupNode::~GroupNode()
	{
	}

	void GroupNode::Update(TimeStep ts)
	{
	}

	void GroupNode::OnImGuiNodeRender()
	{
	}

	void GroupNode::OnImGuiInspectorRender()
	{
	}

	SerializerValue GroupNode::Save()
	{
		SerializerValue value = JSONSerializer::CreateValue();
		SerializerObject object = JSONSerializer::CreateObjectFromValue(value);
		
		Node::SaveCore(object);

		return value;
	}

	void GroupNode::Load(SerializerObject& obj)
	{
	}

}