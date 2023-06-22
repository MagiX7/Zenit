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
		SetData(data);
	}

	Texture2D::Texture2D(const std::string& path) : path(path), width(0), height(0)
	{
		int w, h, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &w, &h, &channels, 0);

		this->data = data;

		name = path.substr(path.find_last_of("/") + 1, path.length());

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
			std::cout << "[INFO] Texture format not supported" << std::endl;
			return;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTextureStorage2D(id, 1, storageFormat, width, height);

		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(id, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);

	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &id);
	}

	void Texture2D::SetData(uint32_t* data)
	{
		if (id > 0)
			glDeleteTextures(1, &id);

		this->data = data;
		glCreateTextures(GL_TEXTURE_2D, 1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);		

	}

	void Texture2D::Bind(uint32_t slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void Texture2D::BindImage()
	{
		glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	}

	void Texture2D::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::UnbindImage()
	{
		glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	}

}