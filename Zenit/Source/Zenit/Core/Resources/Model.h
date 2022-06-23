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

		void Update(TimeStep ts, float dx, float dy);
		void Draw() const;
		void ResetRotation();

		inline const glm::mat4& GetTransform() { return transform; }

	private:
		std::string path;
		std::vector<Mesh*> meshes;
		
		glm::mat4 transform;
		glm::quat rotation;
		
		friend class ModelImporter;
	};

}