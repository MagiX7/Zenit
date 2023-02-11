#pragma once

#include "Mesh.h"
#include "Zenit/Scene/Entity.h"
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

	class Model : public Entity
	{
	public:
		Model(std::string path);
		~Model();

		void Update(TimeStep ts, float dx, float dy);
		void Draw(const std::unique_ptr<Shader>& shader) const;
		void ResetRotation();

		void AddMesh(Mesh* mesh);

		void SetPosition(const glm::vec3& pos) { transform[3] = glm::vec4(pos, 1); }

		inline const glm::mat4& GetTransform() { return transform; }
		inline const std::string& GetName() { return name; }
		inline AABB& GetAABB() { return aabb; }

		inline const std::vector<Mesh*>& GetMeshes() { return meshes; }

	private:
		std::string path;
		std::string name;
		std::vector<Mesh*> meshes;
		AABB aabb;

		glm::mat4 transform;
		glm::quat rotation;

	};

}