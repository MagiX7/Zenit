#include "Zenit/Core/Application.h"
#include "Zenit/Core/Window.h"
#include "Zenit/Core/Input.h"

#include "Model.h"
#include "Zenit/Renderer/PerspectiveCamera.h"

#include "Zenit/Renderer/Shader.h"
#include "Zenit/Renderer/Texture2D.h"

#include "Zenit/Core/TimeStep.h"
#include "Zenit/Core/Log.h"

#include "Zenit/Renderer/Skybox.h"

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/matrix_decompose.hpp>


namespace Zenit {

	Model::Model(std::string path) : path(path)
	{
		rotation = glm::vec3(0, 0, 0);
		UpdateTransform();

		int start = path.find_last_of("\\") + 1;
		int end = path.find_last_of(".");
		name = path.substr(start, end - start);

		aabb = AABB();
	}

	Model::~Model()
	{
	}

	void Model::Update(TimeStep ts, float dx, float dy)
	{
		static bool firstInput = true;

		if (!Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
		{
			firstInput = true;
		}
		else if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
		{
			if (firstInput)
			{
				firstInput = false;
				return;
			}

			if (dy)
			{
				glm::vec3 right = glm::vec3(transform[0][0], transform[1][0], transform[2][0]);
				transform = glm::rotate(transform, -dy * ts, right);
				glm::extractEulerAngleXYZ(transform, rotation.x, rotation.y, rotation.z);
				//aabb.Transform(transform, -dy * ts, right);
			}
			if (dx)
			{
				glm::vec3 up = glm::vec3(transform[0][1], transform[1][1], transform[2][1]);
				transform = glm::rotate(transform, dx * ts, up);
				glm::extractEulerAngleXYZ(transform, rotation.x, rotation.y, rotation.z);
				//aabb.Transform(transform, dx * ts, up);
			}
		}
	}

	void Model::Draw(const std::unique_ptr<Shader>& shader) const
	{
		for (auto& m : meshes)
			m->Draw(shader);
	}

	/*void Model::ResetRotation()
	{
		rotation = glm::vec3(0);
		UpdateTransform();
	}*/

	void Model::AddMesh(Mesh* mesh)
	{
		meshes.push_back(mesh);
		std::vector<Vertex> vertices = mesh->GetVertices();
		std::vector<glm::vec3> positions;

		positions.resize(vertices.size());
		for (size_t i = 0; i < vertices.size(); ++i)
			positions[i] = vertices[i].position;

		aabb.Extend(mesh->GetAABB());
		//aabb.RefreshData();
	}

	void Model::UpdateTransform()
	{
		transform = glm::translate(glm::mat4(1.0), { 0,0,0 })
			* glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	}

	void Model::ResetTransform()
	{
		rotation = glm::vec3(0.0f);
		transform = glm::mat3(1.0f);
	}

}