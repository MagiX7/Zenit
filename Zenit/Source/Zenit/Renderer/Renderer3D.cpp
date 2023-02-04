#include "Renderer3D.h"
#include "Zenit/Core/Log.h"

#include "VertexBuffer.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Zenit {

	Renderer3D* Renderer3D::instance = nullptr;

	Renderer3D::Renderer3D()
	{
		
	}

	Renderer3D::~Renderer3D()
	{
	}

	Renderer3D* Renderer3D::GetInstance()
	{
		if (!instance)
			instance = new Renderer3D();

		return instance;
	}

	void Renderer3D::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);
		glLineWidth(2.0f);
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