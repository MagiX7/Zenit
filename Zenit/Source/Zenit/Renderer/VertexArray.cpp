#include "VertexArray.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <glad/glad.h>

namespace Zenit {

    VertexArray::VertexArray()
    {
        // We do not bind it here because if we later create a vertex array and its bound, they are linked together and that may be dangerous.
        glGenVertexArrays(1, &vao);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &vao);
        vertexBuffers.clear();
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(vao);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    // This should be called AFTER setting the layout to the VertexBuffer.
    void VertexArray::AddVertexBuffer(VertexBuffer* vertexBuf)
    {
        glBindVertexArray(vao);
        vertexBuf->Bind();

        if (vertexBuf->GetLayout().GetElements().size() > 0)
        {
            uint32_t index = 0;
            const auto& layout = vertexBuf->GetLayout();
            int offset = 0;
            for (const auto& element : layout.GetElements())
            {
                glVertexAttribPointer(index, element.GetComponentCount(),
                    GetOpenGLRawTypeFromShaderDataType(element.type),
                    element.normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (void*)(offset * sizeof(GetOpenGLRawTypeFromShaderDataType(element.type))));
                glEnableVertexAttribArray(index);
                offset += element.GetComponentCount();
                index++;
            }
            vertexBuffers.push_back(vertexBuf);
        }
    }

    void VertexArray::SetIndexBuffer(IndexBuffer* indexBuf)
    {
        glBindVertexArray(vao);
        indexBuf->Bind();
        indexBuffer = indexBuf;
    }

}