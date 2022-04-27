#include "Application.h"
#include "Window.h"

#include "Input.h"

#include <GLFW/glfw3.h>

namespace Zenit {

	bool Input::IsKeyPressed(int keycode)
	{
		GLFWwindow* window = Application::GetInstance().GetWindow().GetNativeWindow();
		int state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		GLFWwindow* window = Application::GetInstance().GetWindow().GetNativeWindow();

		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	float Input::GetMouseX()
	{
		GLFWwindow* window = Application::GetInstance().GetWindow().GetNativeWindow();

		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return (float)x;
	}

	float Input::GetMouseY()
	{
		GLFWwindow* window = Application::GetInstance().GetWindow().GetNativeWindow();

		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return (float)y;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		GLFWwindow* window = Application::GetInstance().GetWindow().GetNativeWindow();

		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return std::pair(x, y);
	}

}