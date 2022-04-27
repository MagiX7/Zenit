#pragma once

#include "GLFW/glfw3.h"

#include "Zenit/Events/Event.h"

#include <string>
#include <functional>

namespace Zenit {

	class Window
	{
	public:

		using EventCallbackFn = std::function<void(Event&)>;

		Window(int width, int height, const std::string& title = "Zenit");
		virtual ~Window();

		void Update();

		inline void SetEventCallback(const EventCallbackFn& callback) { props.eventCallback = callback; }

		bool ShouldClose();


		inline void SetTitle(const std::string& title) { props.title = title; }

		inline int GetWidth() const { return props.width; }
		inline int GetHeight() const { return props.height; }

		inline GLFWwindow* GetNativeWindow() const { return window; }

	private:
		bool Init();
		void Shutdown();

	private:
		GLFWwindow* window;

		struct WindowProperties
		{
			std::string title = "";
			int width = 0;
			int height = 0;
			bool vsync;

			EventCallbackFn eventCallback;
		};
		WindowProperties props;
	};

}