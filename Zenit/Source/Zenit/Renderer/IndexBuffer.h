#pragma once

#include <glad/glad.h>
#include "glm/gtc/type_ptr.hpp"

#include <utility>

namespace Zenit {

	class IndexBuffer
	{
	public:
		IndexBuffer();
		IndexBuffer(const uint32_t* indices, uint32_t count);
		virtual ~IndexBuffer();

		void SetData(const uint32_t* indices, uint32_t count);
		void Bind() const;
		void Unbind() const;

		inline const uint32_t& GetCount() const { return count; }
		inline const GLuint& GetID() const { return ibo; }


	private:
		GLuint ibo;
		uint32_t count;
	};


}