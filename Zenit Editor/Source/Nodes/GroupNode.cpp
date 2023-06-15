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
		//JSONSerializer::SetNumber(object, "bucketCount", bucketIds.size());
		//for (int i = 0; i < bucketIds.size(); ++i)
		//	JSONSerializer::SetNumber(object, std::to_string(i).c_str(), bucketIds[i]);

		return value;
	}

	void GroupNode::Load(SerializerObject& obj)
	{
		//int count = JSONSerializer::GetNumberFromObject(obj, "bucketCount");
		//bucketIds.resize(count);
		//for (int i = 0; i < count; ++i)
		//	bucketIds[i] = JSONSerializer::GetNumberFromObject(obj, std::to_string(i).c_str());
	}

}