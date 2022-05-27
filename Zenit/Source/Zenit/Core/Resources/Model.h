#pragma once

#include "Mesh.h"
#include "Zenit/Core/TimeStep.h"

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

		void Update(TimeStep ts);
		void Draw(PerspectiveCamera& camera, const std::unique_ptr<Skybox>& skybox);
		void ResetRotation();

		inline const glm::mat4& GetTransform() { return transform; }

	private:
		std::string path;
		std::vector<Mesh*> meshes;
		Texture2D* diffuse;

		glm::mat4 transform;
		glm::quat rotation;

		Shader* shader;

		friend class ModelImporter;
	};

}