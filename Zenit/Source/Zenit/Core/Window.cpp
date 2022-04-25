#include "Window.h"

#include <glad/glad.h>
#include <iostream>

namespace Zenit {

	Window::Window()
	{
	}

	Window::~Window()
	{
	}

	void Window::Init()
	{
		/* Initialize the library */
		if (!glfwInit())
			std::cout << "GLFW Init failed" << std::endl;

		/* Create a windowed mode window and its OpenGL context */
		window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			std::cout << "GLFW Window creation failed" << std::endl;
		}

		/* Make the window's context current */
		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			std::cout << "Failed to initialize OpenGL context" << std::endl;

	}

	void Window::Update()
	{
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void Window::CleanUp()
	{
		glfwTerminate();
	}
}