#include "Node.h"
#include "Panels/PanelNodes.h"

namespace Zenit {

	Texture2D* Node::white = nullptr;

	Node::Node(int id, const char* name, NodeOutputType outputType, ImColor color)
		: id(id), name(name), headerColor(color), outputType(outputType), size(5, 5)
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
		//texture->UnbindImage();
	}

	void Node::DispatchCompute(int xPixels, int yPixels) const
	{
		glDispatchCompute(texture->GetWidth() / xPixels, texture->GetHeight() / yPixels, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		computeShader->Unbind();
	}

	Texture2D* Node::GetWhite()
	{
		if (!white) white = new Texture2D("Settings/white.png");
		return white;
	}

}