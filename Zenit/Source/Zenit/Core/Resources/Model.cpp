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
		glm::mat4 transform = glm::translate(glm::mat4(1.0), { 0,0,0 })
			* glm::toMat4(glm::quat(0, 0, 0, 1))
			* glm::scale(glm::mat4(1), glm::vec3(0.1));

		shader->SetUniformMatrix4f("model", transform);

		for (auto& m : meshes)
		{
			m->Draw();
		}

		shader->Unbind();
	}

}