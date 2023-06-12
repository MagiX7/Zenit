#pragma once

#include "Vertex.h"
#include "BufferLayout.h"

#include "glad/glad.h"
#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Zenit {

	class VertexBuffer
	{
	public:
		VertexBuffer();
		VertexBuffer(float* vertices, int count);
		VertexBuffer(Vertex* vertices, uint32_t size);
		virtual ~VertexBuffer();

		//void SetData(void* vertices, uint32_t size);
		//void SetData(const std::vector<Vertex>&);
		void Bind() const;
		void Unbind() const;

		inline const uint32_t& GetCount() const { return count; }

		inline const BufferLayout& GetLayout() const { return layout; }

		// Make sure to do this BEFORE adding the VertexBuffer into the VertexArray.
		inline void SetLayout(const BufferLayout& l) { layout = l; }

	private:
		GLuint vbo;
		uint32_t count;
		BufferLayout layout;
	};

}