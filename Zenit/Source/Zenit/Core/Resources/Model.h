#pragma once

#include "Mesh.h"

#include <string>
#include <vector>

namespace Zenit {

	class Shader;
	class PerspectiveCamera;

	class Model
	{
	public:
		Model(std::string path);
		~Model();

		void Draw(PerspectiveCamera& camera);

	private:
		std::string path;
		std::vector<Mesh*> meshes;

		Shader* shader;

		friend class ModelImporter;
	};

}