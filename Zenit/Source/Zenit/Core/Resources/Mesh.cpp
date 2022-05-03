#include "Mesh.h"

#include "Zenit/Renderer/VertexArray.h"
#include "Zenit/Renderer/VertexBuffer.h"
#include "Zenit/Renderer/IndexBuffer.h"

namespace Zenit {

	Mesh::Mesh(std::vector<Vertex> verts, std::vector<uint32_t> inds) : vertices(verts), indices(inds)
	{
		SetBuffers();
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::SetBuffers()
	{
		vao = std::make_shared<VertexArray>();

		vbo = std::make_shared<VertexBuffer>(vertices, vertices.size() * sizeof(Vertex));
		vbo->SetLayout({
			{ShaderDataType::VEC3F, "psoition"},
			{ShaderDataType::VEC3F, "normals"},
			{ShaderDataType::VEC2F, "texCoords"},
		});
		vao->AddVertexBuffer(&*vbo);

		ibo = std::make_shared<IndexBuffer>(indices, indices.size());
		vao->SetIndexBuffer(&*ibo);
	}

}