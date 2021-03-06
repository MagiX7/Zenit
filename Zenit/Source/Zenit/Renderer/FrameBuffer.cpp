#include "Framebuffer.h"
#include "Zenit/Core/Log.h"

#include <glad/glad.h>

namespace Zenit {
	FrameBuffer::FrameBuffer(int w, int h, int channel) : width(w), height(h), channelId(channel)
	{
		framebuffer = 0;
		colorTexture = 0;
		rboDepthStencil = 0;
		SetFramebuffer();
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &colorTexture);
		glDeleteRenderbuffers(1, &rboDepthStencil);
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, width, height);
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::SetFramebuffer()
	{
		GLenum er = glGetError();
		if (er) ZN_CORE_ERROR("OpenGL error {0}", glGetString(er));

		if (framebuffer > 0)
		{
			glDeleteFramebuffers(1, &framebuffer);
			glDeleteTextures(1, &colorTexture);
			glDeleteTextures(1, &normalTexture);
			glDeleteTextures(1, &depthTexture);
			glDeleteRenderbuffers(1, &rboDepthStencil);
		}


		
		if (er = glGetError()) ZN_CORE_ERROR("OpenGL error {0}", glGetString(er));

		glGenFramebuffers(1, &framebuffer);
		glGenRenderbuffers(1, &renderedBufferRenderer);
		glGenRenderbuffers(1, &depthRenderbuffer);


		er = glGetError();
		if (er)
			ZN_CORE_ERROR("OpenGL error {0}", glGetString(er));

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderedBufferRenderer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderedBufferRenderer);

		//GLenum er;
		er = glGetError();
		if (er)
			ZN_CORE_ERROR("OpenGL error {0}", glGetString(er));

		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

		er = glGetError();
		if (er)
			ZN_CORE_ERROR("OpenGL error {0}", glGetString(er));

		glGenTextures(1, &colorTexture);
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		er = glGetError();
		if (er)
			ZN_CORE_ERROR("OpenGL error {0}", glGetString(er));

		// Attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

		glGenTextures(1, &normalTexture);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		er = glGetError();
		if (er)
			ZN_CORE_ERROR("OpenGL error {0}", glGetString(er));

		// Attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);

		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		er = glGetError();
		if (er)
			ZN_CORE_ERROR("OpenGL error {0}", glGetString(er));

		// Attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

		//GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT };
		
		//glDrawBuffers(3, buffers);
		er = glGetError();
		if (er)
			ZN_CORE_ERROR("OpenGL error {0}", glGetString(er));

		glBindTexture(GL_TEXTURE_2D, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		er = glGetError();
		if (er)
			ZN_CORE_ERROR("OpenGL error {0}", glGetString(er));

		GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (err != GL_FRAMEBUFFER_COMPLETE)
			ZN_CORE_ERROR("Framebuffer is Incomplete. Error {0}", glGetString(err));
		else
			ZN_CORE_INFO("Framebuffer is Complete");

	}

	void FrameBuffer::Resize(int w, int h)
	{
		if (w != width || height != h)
		{
			width = w;
			height = h;
			SetFramebuffer();
		}
	}

	void FrameBuffer::GetColorData(unsigned int* buffer, int channels)
	{
		buffer = new unsigned int[width * height * channels];
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	}

	void FrameBuffer::GetNormalsData(void* buffer)
	{
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &buffer);
	}

	void FrameBuffer::GetDepthData(void* buffer)
	{
		glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, &buffer);
	}
}
