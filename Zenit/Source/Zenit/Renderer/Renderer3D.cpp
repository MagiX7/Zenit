#include "Renderer3D.h"

#include <glad/glad.h>

namespace Zenit {

	void Renderer3D::Init()
	{
	}

	void Renderer3D::ShutDown()
	{
	}

	void Renderer3D::Clear(const glm::vec4& color)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(color.r, color.g, color.b, color.a);
	}


}