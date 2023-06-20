#pragma once

#include "Mesh.h"
#include "Zenit/Core/TimeStep.h"
#include "Zenit/Utils/AABB.h"

#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace Zenit {

	class Shader;
	class PerspectiveCamera;
	class Texture2D;
	class Skybox;

	class Model
	{
	public:
		Model(std::string path);
		~Model();

		void Update(TimeStep ts, float dx, float dy);
		void Draw() const;

		void AddMesh(Mesh* mesh);

		void SetPosition(const glm::vec3& pos) { transform[3] = glm::vec4(pos, 1); }
		void SetRotation(const glm::vec3& rot) { rotation = rot; UpdateTransform(); }

		inline const glm::vec3& GetPosition() { return transform[3]; }
		inline const glm::vec3& GetRotation() { return rotation; }

		void ResetTransform();

		inline const glm::mat4& GetTransform() { return transform; }
		inline const std::string& GetName() { return name; }
		inline AABB& GetAABB() { return aabb; }

		inline const std::vector<Mesh*>& GetMeshes() { return meshes; }

	private:
		void UpdateTransform();

	private:
		std::string path;
		std::string name;
		std::vector<Mesh*> meshes;
		AABB aabb;

		glm::mat4 transform;
		glm::vec3 rotation;

	};

}