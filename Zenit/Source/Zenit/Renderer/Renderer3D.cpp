#include "Renderer3D.h"

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

}