#pragma once

#include <string>

namespace Zenit {

	class Texture2D
	{
	public:
		Texture2D(const std::string& path);
		~Texture2D();

		void Bind(uint32_t slot = 0);

		inline unsigned int GetWidth() { return width; }
		inline unsigned int GetHeight() { return height; }

		inline uint32_t GetId() { return rendererId; }

	private:
		uint32_t rendererId;

		std::string path;
		unsigned int width;
		unsigned int height;
	};

}