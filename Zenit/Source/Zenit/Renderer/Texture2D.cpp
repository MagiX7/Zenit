#include "Texture2D.h"

#include "Zenit/Core/Log.h"

#include <stb_image.h>
#include <glad/glad.h>

#include <iostream>


namespace Zenit {

	Texture2D::Texture2D(uint32_t* data, int w, int h)
	{
		width = w;
		height = h;

		glCreateTextures(GL_TEXTURE_2D, 1, &rendererId);
		glBindTexture(GL_TEXTURE_2D, rendererId);
		glTextureStorage2D(rendererId, 1, GL_RGBA8, width, height);

		glTextureParameteri(rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(rendererId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(rendererId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(rendererId, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(rendererId, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	Texture2D::Texture2D(const std::string& path) : path(path), width(0), height(0)
	{
		int w, h, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
		
		if (!data)
		{
			ZN_CORE_ERROR("Texture {0} couldn't be loaded.", path);
			return;
		}

		width = w;
		height = h;

		GLint storageFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			storageFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			storageFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		if (!storageFormat || !dataFormat)
		{
			std::cout << "[INFO]: Texture format not supported" << std::endl;
			return;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &rendererId);
		glBindTexture(GL_TEXTURE_2D, rendererId);
		glTextureStorage2D(rendererId, 1, storageFormat, width, height);

		glTextureParameteri(rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(rendererId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(rendererId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(rendererId, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(rendererId, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &rendererId);
	}

	void Texture2D::Bind(uint32_t slot)
	{
		//glBindTexture(GL_TEXTURE_2D, rendererId);
		glBindTextureUnit(slot, rendererId);
	}

}