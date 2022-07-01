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

	enum class NodeOutputType
	{
		FLAT_COLOR,
		TEXTURE,
	};

	enum class NodeType
	{
		COLOR,
		PERLIN_NOISE,
		VORONOI,
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
		NodeOutputType outputType;
		NodeType type; // To be defined in each node
		ImVec2 pos;
		ImVec2 size;

		std::string state;
		std::string savedState;

		bool isOutput; // Output written into the mesh

		Node(int id, const char* name, NodeOutputType outputType, ImColor color = ImColor(255, 255, 255))
			: id(id), name(name), nodeColor(color), outputType(outputType), size(5, 5), isOutput(false)
		{
		}

		virtual void Update(TimeStep ts) = 0;
		virtual void OnImGuiRender() = 0;
	};

}
