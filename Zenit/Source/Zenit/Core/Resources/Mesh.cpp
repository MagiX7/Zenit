#include "Mesh.h"

#include "Zenit/Renderer/VertexArray.h"
#include "Zenit/Renderer/VertexBuffer.h"
#include "Zenit/Renderer/IndexBuffer.h"

namespace Zenit {

	Mesh::Mesh(const char* meshName, std::vector<Vertex> verts, std::vector<uint32_t> inds) : name(meshName), vertices(verts), indices(inds)
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

		vbo = std::make_shared<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
		vbo->SetLayout({
			{ShaderDataType::VEC3F, "position"},
			{ShaderDataType::VEC3F, "normals"},
			{ShaderDataType::VEC2F, "texCoords"},
			{ShaderDataType::VEC3F, "tangents"},
			{ShaderDataType::VEC3F, "biTangents"},
		});
		vao->AddVertexBuffer(&*vbo);

		ibo = std::make_shared<IndexBuffer>(&indices[0], indices.size());
		vao->SetIndexBuffer(&*ibo);


		aabb = AABB();
		for (int i = 0; i < vertices.size(); ++i)
			aabb.Extend(vertices[i].position);
	}
}