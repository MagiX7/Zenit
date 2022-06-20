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

		void GetColorData(unsigned int* buffer, int channels);
		void GetNormalsData(void* buffer);
		void GetDepthData(void* buffer);

		inline GLuint GetId() const { return framebuffer; }
		inline GLuint GetColorId() const { return colorTexture; }
		inline GLuint GetNormalsId() const { return normalTexture; }
		inline GLuint GetDepthId() const { return depthTexture; }
		inline int GetWidth() const { return width; }
		inline int GetHeight() const { return height; }

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