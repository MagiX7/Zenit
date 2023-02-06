#include "Node.h"
#include "Panels/PanelNodes.h"

namespace Zenit {

	Node::Node(int id, const char* name, NodeOutputType outputType, ImColor color)
		: id(id), name(name), headerColor(color), outputType(outputType), size(5, 5), isOutput(false)
	{
	}

	Node::~Node()
	{
		// Since the members are not heap allocated, we can only clear the vector without deleting the elements on it
		inputs.clear();
		outputs.clear();

		name.clear();
	}

}