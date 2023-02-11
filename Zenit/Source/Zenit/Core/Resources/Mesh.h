#pragma once

#include "Zenit/Renderer/Vertex.h"
#include "Zenit/Utils/AABB.h"

#include <glm/vec3.hpp>

#include <vector>
#include <memory>
#include <string>

namespace Zenit {

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;

	class Mesh
	{
	public:
		Mesh(const char* meshName, std::vector<Vertex> verts, std::vector<uint32_t> inds);
		~Mesh();

		void Draw(const std::unique_ptr<Shader>&);

		inline const std::vector<Vertex>& GetVertices() { return vertices; }
		inline AABB& GetAABB() { return aabb; }

		inline void SetName(const std::string& value) { name = value; }
		inline const std::string& GetName() { return name; }

	private:
		void SetBuffers();

	private:
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		AABB aabb;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<IndexBuffer> ibo;
	};

}