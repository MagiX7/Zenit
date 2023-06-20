#include "Node.h"
#include "Panels/PanelNodes.h"

#include "../Helpers/NodeHelpers.h"

namespace Zenit {

	Texture2D* Node::white = nullptr;

	Node::Node(int id, const char* name, ImColor color)
		: id(id), name(name), headerColor(color), size(5, 5)
	{
	}

	Node::~Node()
	{
		// Since the members are not heap allocated, we can only clear the vector without deleting the elements on it
		inputs.clear();
		outputs.clear();

		name.clear();
	}

	void Node::Update(TimeStep ts)
	{
		for (auto id : nextNodesIds)
		{
			auto nextNode = NodeHelpers::FindNode(id, PanelNodes::GetNodes());
			if (nextNode)
				nextNode->ForceRegeneration();
		}
	}

	void Node::BindCoreData() const
	{
		texture->BindImage();
		computeShader->Bind();
		computeShader->SetUniform1i("imgOutput", 0);
	}

	void Node::DispatchCompute(int xPixels, int yPixels) const
	{
		computeShader->Dispatch(texture->GetWidth() / xPixels, texture->GetHeight() / yPixels, 1);
		computeShader->Unbind();
	}

	Texture2D* Node::GetWhite()
	{
		if (!white) white = new Texture2D("Settings/white.png");
		return white;
	}

	void Node::SaveCore(SerializerObject& obj)
	{
		JSONSerializer::SetString(obj, "name", name.c_str());
		JSONSerializer::SetNumber(obj, "id", id.Get());
		JSONSerializer::SetNumber(obj, "type", (int)type);

		SerializerValue idsArrayValue = JSONSerializer::CreateArrayValue();
		SerializerArray idsArray = JSONSerializer::CreateArrayFromValue(idsArrayValue);
		JSONSerializer::SetObjectValue(obj, "nextNodesIds", idsArrayValue);
		for (int i = 0; i < nextNodesIds.size(); ++i)
		{
			JSONSerializer::AppendNumberToArray(idsArray, nextNodesIds[i].Get());
			/*SerializerValue value = JSONSerializer::CreateValue();
			SerializerObject object = JSONSerializer::CreateObjectFromValue(value);
			JSONSerializer::SetNumber(object, ("id" + std::to_string(i)).c_str(), nextNodesIds[i].Get());
			JSONSerializer::AppendValueToArray(idsArray, value);*/
		}
	}

}