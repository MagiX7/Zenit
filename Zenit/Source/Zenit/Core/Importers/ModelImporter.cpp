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

            if (mesh->HasTangentsAndBitangents())
            {
                vertex.tangents.x = mesh->mTangents[i].x;
                vertex.tangents.y = mesh->mTangents[i].y;
                vertex.tangents.z = mesh->mTangents[i].z;

                vertex.biTangents.x = mesh->mBitangents[i].x;
                vertex.biTangents.y = mesh->mBitangents[i].y;
                vertex.biTangents.z = mesh->mBitangents[i].z;
            }

            vertices.push_back(vertex);
        }
        
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (!mesh->HasTangentsAndBitangents())
        {
            ComputeTangentsAndBiTangents(vertices, mesh->mNumFaces);
        }

        return new Mesh(vertices, indices);
    }

    void ModelImporter::ComputeTangentsAndBiTangents(std::vector<Vertex>& vertices, unsigned int indicesCount)
    {
        for (int i = 0; i < indicesCount; i += 3)
        {
            glm::vec2 uv1 = { vertices[i].texCoords };
            glm::vec2 uv2 = { vertices[i + 1].texCoords };
            glm::vec2 uv3 = { vertices[i + 2].texCoords };

            glm::vec2 deltaUv1 = uv2 - uv1;
            glm::vec2 deltaUv2 = uv3 - uv1;

            glm::vec3 edge1 = vertices[i + 1].position - vertices[i].position;
            glm::vec3 edge2 = vertices[i + 2].position - vertices[i].position;

            float f = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv2.x * deltaUv1.y);

            vertices[i].tangents.x = f * (deltaUv2.y * edge1.x - deltaUv1.y * edge2.x);
            vertices[i].tangents.y = f * (deltaUv2.y * edge1.y - deltaUv1.y * edge2.y);
            vertices[i].tangents.z = f * (deltaUv2.y * edge1.z - deltaUv1.y * edge2.z);
            vertices[i].tangents = glm::normalize(vertices[i].tangents);

            if (i + 3 > indicesCount)
                break;
        }
    }
}
