#pragma once

#include "Zenit.h"

#include <imgui_node_editor.h>
#include <string>
#include <vector>

namespace ed = ax::NodeEditor;

namespace Zenit {

	enum class PinType
	{
		Flow,
		Bool,
		Int,
		Float,
		String,
		Object,
		Function,
		Delegate,
	};

	enum class PinKind
	{
		Output,
		Input
	};

	enum class NodeType
	{
		FLAT_COLOR,
		TEXTURE,
	};

	struct Pin;

	struct LinkInfo
	{
		ed::LinkId id;
		ed::PinId  inputId;
		ed::PinId  outputId;
	};

	struct Node;

	struct Pin
	{
		ed::PinId id;
		Node* node;
		std::string name;
		PinType type;
		ed::PinKind kind;

		std::vector<LinkInfo> links;

		Pin(int pinId, const char* pinName, PinType type, ed::PinKind kind)
			: id(pinId), node(nullptr), name(pinName), type(type), kind(kind)
		{
		}
	};
	
	struct Node
	{
		ed::NodeId id;
		std::string name;
		std::vector<Pin> inputs;
		std::vector<Pin> outputs;

		ImColor nodeColor;
		NodeType type;
		ImVec2 pos;
		ImVec2 size;

		std::string state;
		std::string savedState;

		bool isOutput; // Output written into the mesh

		Node(int id, const char* name, NodeType type, ImColor color = ImColor(255, 255, 255))
			: id(id), name(name), nodeColor(color), type(type), size(5, 5), isOutput(false)
		{
		}

	};

}
