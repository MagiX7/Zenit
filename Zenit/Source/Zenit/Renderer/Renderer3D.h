#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace Zenit {

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;

	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec2 texCoords;
		glm::vec4 color;
	};

	struct Renderer3DData
	{
		std::shared_ptr<VertexArray> vertexArray;
		std::shared_ptr<VertexBuffer> vertexBuffer;
		std::shared_ptr<IndexBuffer> indexBuffer;
		std::unique_ptr<Shader> shader;

		uint32_t maxQuads = 10000;
		uint32_t maxVertices = 10000 * 4;
		uint32_t maxIndices = 10000 * 6;

		uint32_t indexCount = 0;

		QuadVertex* vertexBufferBase = nullptr;
		QuadVertex* vertexBufferPtr = nullptr;
	};
	

	class Renderer3D
	{
	public:
		// Set up the batch renderer
		static void Init();
		static void ShutDown();

		static void Clear(const glm::vec4& color);


	private:
		static Renderer3DData data;

	};

}