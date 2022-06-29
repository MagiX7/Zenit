#pragma once

#include <glm/vec3.hpp>
#include <stdint.h>

namespace Zenit {

	namespace Math {

		inline uint32_t GetRGBAHexadecimal(glm::vec3 color)
		{
			int r = color.r * 255;
			int g = color.g * 255;
			int b = color.b * 255;
			return 0xff000000 | (b << 16) | (g << 8) | r;
		}

	}

}
