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
		
		JSONSerializer::SetString(object, "name", name.c_str());
		JSONSerializer::SetNumber(object, "id", id.Get());
		JSONSerializer::SetNumber(object, "type", (int)type);

		return value;
	}

	void GroupNode::Load(SerializerObject& obj)
	{
	}

}