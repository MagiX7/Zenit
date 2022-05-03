#include "VertexBuffer.h"

namespace Zenit {

	VertexBuffer::VertexBuffer() : count(0)
	{
		glGenBuffers(1, &vbo);
	}

	VertexBuffer::VertexBuffer(const Vertex* vertices, uint32_t size)
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		count = size / sizeof(uint32_t);
	}

	// Batch rendering purposes
	VertexBuffer::VertexBuffer(uint32_t size)
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &vbo);
	}

	void VertexBuffer::SetData(const void* vertices, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		count =  size / sizeof(uint32_t);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}

