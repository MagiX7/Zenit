#include "ModelImporter.h"
#include "Zenit/Core/Log.h"

namespace Zenit {
 
    Model* ModelImporter::ImportModel(std::string path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            ZN_CORE_ERROR("Assimp error: {0}", importer.GetErrorString());
            return nullptr;
        }

        Model* model = new Model(path);
        ProcessNode(scene->mRootNode, scene, *model);

        ZN_CORE_TRACE("Model {0} loaded", path.c_str());

        return model;
    }

    void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, Model& model)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            model.meshes.push_back(ProcessMesh(mesh, scene));
        }
        
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, model);
        }
    }

    Mesh* ModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        //vertices.resize(mesh->mNumVertices);
        //indices.resize(mesh->mNumFaces);
        
        for (int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex;

            vertex.position.x = mesh->mVertices[i].x;
            vertex.position.y = mesh->mVertices[i].y;
            vertex.position.z = mesh->mVertices[i].z;

            if (mesh->HasNormals())
            {
                vertex.normals.x = mesh->mNormals[i].x;
                vertex.normals.y = mesh->mNormals[i].y;
                vertex.normals.z = mesh->mNormals[i].z;
            }

            if (mesh->mTextureCoords[0])
            {
                vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
                vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
            }
            else
            {
                vertex.texCoords = { 0,0 };
            }

            vertices.push_back(vertex);

            // Load textures

        }
        
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        return new Mesh(vertices, indices);
    }
}
