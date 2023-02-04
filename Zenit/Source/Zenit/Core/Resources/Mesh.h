#pragma once

#include "Zenit/Renderer/Vertex.h"
#include "Zenit/Utils/AABB.h"

#include <glm/vec3.hpp>
#include <vector>
#include <memory>

namespace Zenit {

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;

	class Mesh
	{
	public:
		Mesh(std::vector<Vertex> verts, std::vector<uint32_t> inds);
		~Mesh();

		void Draw();

		inline const std::vector<Vertex>& GetVertices() { return vertices; }
		inline AABB& GetAABB() { return aabb; }

	private:
		void SetBuffers();

	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		AABB aabb;

		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<IndexBuffer> ibo;
	};

}