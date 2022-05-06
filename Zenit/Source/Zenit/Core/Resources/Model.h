#pragma once

#include "Mesh.h"

#include <string>
#include <vector>

namespace Zenit {

	class Model
	{
	public:
		Model(std::string path);
		~Model();

	private:
		std::string path;
		std::vector<Mesh*> meshes;

		friend class ModelImporter;
	};

}