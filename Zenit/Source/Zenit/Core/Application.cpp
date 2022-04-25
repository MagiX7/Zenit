#include "Application.h"
#include "Window.h"
#include "Log.h"

#include <glad/glad.h>

namespace Zenit {

	Application* Application::instance = nullptr;

	Application::Application()
	{
		instance = this;
		window = new Window();
		window->Init();

		Log::Init();
		ZN_CORE_INFO("Logging system initialized");
		ZN_WARN("Client warn");
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