#include "Renderer3D.h"
#include "Zenit/Core/Log.h"

#include <glad/glad.h>

namespace Zenit {

	void Renderer3D::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Renderer3D::ShutDown()
	{
	}

	void Renderer3D::Clear(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer3D::OnResize(int width, int height)
	{
		glViewport(0, 0, width, height);
	}
}