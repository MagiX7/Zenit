#include "Application.h"
#include "Window.h"

#include "Input.h"

#include "Log.h"

#include <GLFW/glfw3.h>

namespace Zenit {

	Input* Input::instance = nullptr;
	
	Input* Input::GetInstance()
	{
		if (!instance)
			instance = new Input();
		return instance;
	}

	void Input::Shutdown()
	{
		delete instance;
	}

	bool Input::IsKeyPressed(int keycode)
	{
		GLFWwindow* window = Application::GetInstance().GetWindow().GetNativeWindow();
		int state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS;
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
		glfwGetCursorPos(window, &x, &y);
		return (float)x;
	}

	float Input::GetMouseY()
	{
		GLFWwindow* window = Application::GetInstance().GetWindow().GetNativeWindow();
		glfwGetCursorPos(window, &x, &y);
		return (float)y;
	}

	float Input::GetMouseMotionX()
	{
		return (GetMouseX() - lastMousePosX) / Application::GetInstance().GetWindow().GetWidth();
	}

	float Input::GetMouseMotionY()
	{
		return (lastMousePosY - GetMouseY()) / Application::GetInstance().GetWindow().GetHeight();
	}

	glm::vec2 Input::GetMousePosition()
	{
		GLFWwindow* window = Application::GetInstance().GetWindow().GetNativeWindow();

		static double x, y;
		lastMousePosX = x;
		lastMousePosY = y;
		glfwGetCursorPos(window, &x, &y);

		return glm::vec2(x, y);
	}

	void Input::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ZN_BIND_EVENT_FN(Input::OnMouseScrolled));
	}

	void Input::ResetScrollStats()
	{
		deltaXScroll = 0;
		deltaYScroll = 0;
		lastMousePosX = x;
		lastMousePosY = y;
	}

	bool Input::OnMouseScrolled(MouseScrolledEvent& e)
	{
		deltaXScroll = e.GetOffsetX();
		deltaYScroll = e.GetOffsetY();

		return true;
	}
}