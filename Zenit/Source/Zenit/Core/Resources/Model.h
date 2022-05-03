#pragma once

#include <string>
#include <vector>

namespace Zenit {

	class Mesh;

	class Model
	{
	public:
		Model(const char* path);
		~Model();

	private:
		std::string path;
		std::vector<Mesh*> meshes;
	};

}