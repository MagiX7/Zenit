#pragma once

#include "Node.h"

namespace Zenit {

	class GroupNode : public Node
	{
	public:
		GroupNode(int id, const char* name);
		virtual ~GroupNode();

		virtual void Update(TimeStep ts) override;
		virtual void OnImGuiNodeRender() override;
		virtual void OnImGuiInspectorRender() override;

		virtual SerializerValue Save() override;
		virtual void Load(SerializerObject& obj) override;


		//inline void AddNode(int nodeId) { bucketIds.emplace_back(nodeId); }
		//inline const std::vector<int>& GetBucketIds() { return bucketIds; }



	private:
		//std::vector<int> bucketIds;

	};


}
