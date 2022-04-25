#pragma once

#include <GLFW/glfw3.h>

namespace Zenit {

	class Window
	{
	public:
		Window();
		~Window();

		void Init();
		void Update();
		void CleanUp();

		GLFWwindow* GetNativeWindow() { return window; }
	
	private:
		GLFWwindow* window;

	};


}