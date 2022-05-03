#pragma once

#include <glm/glm.hpp>

namespace Zenit {

	class Renderer3D
	{
	public:
		static void Init();
		static void ShutDown();
		static void Clear(const glm::vec4& color);

	};

}