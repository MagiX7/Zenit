#pragma once

#include "Zenit/Utils/AABB.h"

#include <glm/glm.hpp>
#include <memory>

namespace Zenit {

	class VertexBuffer;
	class Shader;

	class Renderer3D
	{
	public:
		static Renderer3D* GetInstance();

		void Init();
		void ShutDown();
		void Clear(const glm::vec4& color);
		void OnResize(int width, int height);

	private:
		Renderer3D();
		virtual ~Renderer3D();

		Renderer3D(const Renderer3D&) = delete;
		Renderer3D& operator=(const Renderer3D&) = delete;

	private:
		static Renderer3D* instance;

	};
}