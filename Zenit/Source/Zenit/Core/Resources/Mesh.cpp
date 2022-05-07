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

	void Mesh::Draw()
	{
		vao->Bind();
		ibo->Bind();
		glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, 0);
		ibo->Unbind();
		vao->Unbind();
	}

	void Mesh::SetBuffers()
	{
		vao = std::make_shared<VertexArray>();

		vbo = std::make_shared<VertexBuffer>(&vertices[0], vertices.size() * sizeof(Vertex));
		vbo->SetLayout({
			{ShaderDataType::VEC3F, "psoition"},
			{ShaderDataType::VEC3F, "normals"},
			{ShaderDataType::VEC2F, "texCoords"},
		});
		vao->AddVertexBuffer(&*vbo);

		ibo = std::make_shared<IndexBuffer>(&indices[0], indices.size());
		vao->SetIndexBuffer(&*ibo);
	}
}