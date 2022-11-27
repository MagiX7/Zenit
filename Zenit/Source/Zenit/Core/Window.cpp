#include "Window.h"

#include "Zenit/Events/ApplicationEvent.h"
#include "Zenit/Events/KeyEvent.h"
#include "Zenit/Events/MouseEvent.h"
#include "Log.h"

#include "glad/glad.h"

#include <iostream>

namespace Zenit {


	static void GLFWErrorCallback(int err, const char* msg)
	{
		std::string error = "[ERROR]: (" + std::to_string(err) + ")" + msg;
		ZN_CORE_ERROR(error);
	}

	// TODO: Add window flags
	Window::Window(int width, int height, const std::string& title)
	{
		props = WindowProperties();
		
		props.width = width;
		props.height = height;
		props.title = title;


		Init();
	}

	Window::~Window()
	{
		Shutdown();		
	}

	bool Window::Init()
	{
		bool ret = true;

		if (!glfwInit())
		{
			ZN_CORE_INFO("[INFO] GLFW Initialization Failed.");
			ret = false;
			return false;
		}
		else
		{
			ZN_CORE_INFO("[INFO] GLFW Initialization Correct.");
		}

		glfwSetErrorCallback(GLFWErrorCallback);

		window = glfwCreateWindow(props.width, props.height, props.title.c_str(), 0, 0);
			
		if (window) ZN_CORE_INFO("[INFO] Window Initialization...");

		glfwMakeContextCurrent(window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (status == 0)
		{
			ZN_CORE_INFO("[INFO] GLAD Loader Failed.");
			return false;
		}
		else
		{
			ZN_CORE_INFO("[INFO] GLAD Loader Correct.");
			ZN_CORE_INFO("[INFO] OpenGL Version: {0}", glGetString(GL_VERSION));
		}

		glfwSetWindowUserPointer(window, &props);

		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			WindowProperties& p = *(WindowProperties*)glfwGetWindowUserPointer(window);

			p.width = width;
			p.height = height;

			WindowResizeEvent e(width, height);
			p.eventCallback(e);
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
		{
			WindowProperties& p = *(WindowProperties*)glfwGetWindowUserPointer(window);

			WindowCloseEvent e;
			p.eventCallback(e);

		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowProperties& p = *(WindowProperties*)glfwGetWindowUserPointer(window);
			
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key, 0);
					p.eventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					p.eventCallback(e);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(key, 1);
					p.eventCallback(e);
					break;
				}
			}
		});

		glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowProperties& p = *(WindowProperties*)glfwGetWindowUserPointer(window);

			KeyTypedEvent e(keycode);
			p.eventCallback(e);
		});


		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowProperties& p = *(WindowProperties*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(button);
					p.eventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent e(button);
					p.eventCallback(e);
					break;
				}
			}
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowProperties& p = *(WindowProperties*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e((float)xOffset, (float)yOffset);
			p.eventCallback(e);
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y)
		{
			WindowProperties& p = *(WindowProperties*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((float)x, (float)y);
			p.eventCallback(e);
		});


		glfwSwapInterval(1);

		return ret;
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(window);
		//glfwTerminate();
	}

	void Window::SwapBuffers()
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(window);
	}	
}