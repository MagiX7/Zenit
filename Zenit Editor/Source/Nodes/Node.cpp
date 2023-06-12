#include "Node.h"
#include "Panels/PanelNodes.h"

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

}