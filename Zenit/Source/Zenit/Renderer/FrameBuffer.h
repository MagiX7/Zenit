#pragma once

typedef unsigned int GLuint;

namespace Zenit {

	class FrameBuffer
	{
	public:
		FrameBuffer(int w, int h, int channel);
		~FrameBuffer();

		void Bind();
		void Unbind();

		void SetFramebuffer();
		void Resize(int w, int h);

		inline const GLuint GetId() const { return framebuffer; }
		inline const GLuint GetColorId() const { return colorTexture; }
		inline const GLuint GetNormalsId() const { return normalTexture; }
		inline const GLuint GetDepthId() const { return depthTexture; }
		inline const int GetWidth() const { return width; }
		inline const int GetHeight() const { return height; }

	private:
		int width;
		int height;
		int channelId;

		GLuint framebuffer;
		GLuint quadFramebuffer;
		GLuint renderedBufferRenderer;
		GLuint depthRenderbuffer;

		GLuint rboDepthStencil;
		GLuint colorTexture;
		GLuint normalTexture;
		GLuint depthTexture;
	};

}