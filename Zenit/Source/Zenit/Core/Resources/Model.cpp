#include "Model.h"
#include "Zenit/Core/PerspectiveCamera.h"

#include "Zenit/Renderer/Shader.h"

namespace Zenit {

	Model::Model(std::string path) : path(path)
	{
		shader = new Shader("Assets/Shaders/default.shader");
	}

	Model::~Model()
	{
	}

	void Model::Draw(PerspectiveCamera& camera)
	{
		shader->Bind();
		shader->SetUniformMatrix4f("view", camera.GetView());
		shader->SetUniformMatrix4f("projection", camera.GetProjection());

		for (auto& m : meshes)
		{
			m->Draw();
		}

		shader->Unbind();
	}

}