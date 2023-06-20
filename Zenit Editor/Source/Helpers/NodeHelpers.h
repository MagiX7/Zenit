#pragma once

#include "../Nodes/Node.h"

namespace Zenit {

	namespace NodeHelpers {

		static Pin incorrectPin = Pin(-1, "", ed::PinKind::None);
		static LinkInfo incorrectLink = LinkInfo(-1, -1, -1);

		static Node* FindNode(ed::NodeId id, const std::vector<Node*> nodes)
		{
			for (const auto& node : nodes)
			{
				if (node->id == id)
					return node;
			}

			return nullptr;
		}

		static Pin* FindPin(ed::PinId id, const std::vector<Node*> nodes)
		{
			for (int i = 0; i < nodes.size(); ++i)
			{
				for (auto& inputPin : nodes[i]->inputs)
				{
					if (inputPin.id == id)
						return &inputPin;
				}

				for (auto& outputPin : nodes[i]->outputs)
				{
					if (outputPin.id == id)
						return &outputPin;
				}
			}

			return &incorrectPin;
		}

		static const LinkInfo& FindLink(const ed::LinkId& id, const std::vector<LinkInfo>& links)
		{			
			for (auto& link : links)
			{
				if (link.id == id)
					return link;
			}

			return incorrectLink;
		}

	}

}