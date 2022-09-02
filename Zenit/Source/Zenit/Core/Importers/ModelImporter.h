#pragma once

#include "Zenit/Core/Resources/Model.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//
//class aiNode;
//class aiScene;
//class aiMesh;

namespace Zenit {

	class ModelImporter
	{
	public:
		static Model* ImportModel(std::string path);

	private:
		static void ProcessNode(aiNode* node, const aiScene* scene, Model& model);
		static Mesh* ProcessMesh(aiMesh* node, const aiScene* scene);
		static void ComputeTangentsAndBiTangents(std::vector<Vertex>& vertices, unsigned int indicesCount);
	};

}