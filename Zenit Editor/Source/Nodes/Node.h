#pragma once

#include "Zenit.h"

#include <imgui_node_editor.h>
#include <string>
#include <vector>

namespace ed = ax::NodeEditor;

#define NODE_TEXTURE_SIZE 2048

namespace Zenit {

	class PanelNodes;

	enum class NodeType
	{
		COLOR = 0,
		GRADIENT,

		// Generators
		CIRCLE,
		CHECKERS,
		WHITE_NOISE,
		FBM_NOISE,
		DERIVATIVE_NOISE,
		GRADIENT_NOISE,
		VORONOI,
		KIFS_FRACTAL,

		// Filters
		NORMAL_MAP,
		TILING,
		EDGE_DETECTOR,
		TWIRL,
		INVERT,

		// Operators
		BLEND,
		CLAMP,
		MAX,
		MIN,
		POW = 19,
		ADD = 20,
		SUBSTRACT = 21,

		// Transform
		TRANSFORM,

		// Helpers
		GROUP,
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
		std::string name;
		ed::PinKind kind;
		Node* node;

		Pin(int pinId, const char* pinName, ed::PinKind kind)
			: id(pinId), node(nullptr), name(pinName), kind(kind)
		{
		}
	};

	class Node
	{
	public:
		Node() = delete;
		Node(int id, const char* name, ImColor color = ImColor(255, 255, 255));

		virtual ~Node();

		virtual void Update(TimeStep ts);
		virtual void ResetDefaultState() {};
		virtual void OnImGuiNodeRender() {};
		virtual void OnImGuiInspectorRender() {};

		void ForceRegeneration() { regenerate = true; }

		void BindCoreData() const;
		void DispatchCompute(int xPixels, int yPixels) const;

		static Texture2D* GetWhite();

		virtual SerializerValue Save() { return SerializerValue(); };
		virtual void Load(SerializerObject& obj) {};

		bool operator==(const Node& other) const
		{
			return id == other.id;
		}
		

	public:
		ed::NodeId id;
		std::string name;
		bool changeName = false;
		std::vector<Pin> inputs;
		std::vector<Pin> outputs;
		std::vector<ed::NodeId> nextNodesIds;// = ed::NodeId(4294967295); // Max uint
		

		ImColor headerColor = { 255,255,255,255 };
		NodeType type; // To be defined in each node
		ImVec2 pos;
		ImVec2 size;

		// Compute Shader parameters
		std::shared_ptr<Texture2D> texture;
		std::unique_ptr<ComputeShader> computeShader;
		static Texture2D* white;

	protected:
		bool regenerate = true;

	};

}
