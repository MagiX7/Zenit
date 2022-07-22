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
		NONE,
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

	class Node;

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

	// TODO: Create cpp and clear all the lists
	class Node
	{
	public:
		Node();
		Node(int id, const char* name, NodeOutputType outputType, ImColor color = ImColor(255, 255, 255))
			: id(id), name(name), nodeColor(color), outputType(outputType), size(5, 5), isOutput(false)
		{
		}

		virtual ~Node() {}		

		virtual void Update(TimeStep ts) {};
		virtual void OnImGuiRender() {};

		bool operator==(const Node& other) const
		{
			return id == other.id;
		}


	public:
		ed::NodeId id;
		std::string name;
		std::vector<Pin> inputs;
		std::vector<Pin> outputs;

		ImColor nodeColor = { 255,255,255,255 };
		NodeOutputType outputType;
		NodeType type; // To be defined in each node
		ImVec2 pos;
		ImVec2 size;

		std::string state;
		std::string savedState;

		bool isOutput; // Output written into the mesh

	};

}
