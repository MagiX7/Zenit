#include "Application.h"
#include "Window.h"

#include <glad/glad.h>

namespace Zenit {

	Application::Application()
	{
		window = new Window();
		window->Init();
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		Start();

		while (!glfwWindowShouldClose(window->GetNativeWindow()))
		{
			Update();


			Draw();
		}

		CleanUp();

	}

	void Application::Start()
	{
	}

	void Application::Update()
	{
	}

	void Application::Draw()
	{
		glClear(GL_COLOR_BUFFER_BIT);


		glfwSwapBuffers(window->GetNativeWindow());
		glfwPollEvents();
	}

	void Application::CleanUp()
	{
	}

}