#pragma once

#include "Zenit.h"

#include <imgui_node_editor.h>
#include <string>
#include <vector>

namespace ed = ax::NodeEditor;

namespace Zenit {

	enum class PinType
	{
		None = -1,
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
		TEXTURE,
		VEC1,
		VEC2,
		VEC3,
	};

	enum class NodeType
	{
		COLOR = 0,
		NOISE,
		PERLIN_NOISE,
		DERIVATIVE_NOISE,
		VORONOI,

		// Filters
		NORMAL_MAP,
		TWIRL,

		// Operators
		BLEND,
		CLAMP,
		MAX,
		MIN,
	};

	struct Pin;

	struct LinkInfo
	{
		LinkInfo(ed::LinkId id, ed::PinId inputId, ed::PinId  outputId)
			: id(id), inputId(inputId), outputId(outputId)
		{
		}
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

		//std::vector<LinkInfo> links;

		Pin(int pinId, const char* pinName, PinType type, ed::PinKind kind)
			: id(pinId), node(nullptr), name(pinName), type(type), kind(kind)
		{
		}
	};

	class Node
	{
	public:
		Node() = delete;
		Node(int id, const char* name, NodeOutputType outputType, ImColor color = ImColor(255, 255, 255));

		virtual ~Node();

		virtual void Update(TimeStep ts) {};
		virtual void ResetDefaultState() {};
		virtual void OnImGuiNodeRender() {};
		virtual void OnImGuiInspectorRender() {};

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
