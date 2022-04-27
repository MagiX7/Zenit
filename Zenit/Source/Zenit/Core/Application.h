#pragma once

#include "Zenit/Events/ApplicationEvent.h"
#include "Zenit/Debug/ImGuiLayer.h"
#include "LayerStack.h"

#include <memory>

namespace Zenit {

	class Window;
	class Event;

	class Application
	{
	public:
		Application();
		virtual ~Application();	
		
		inline static Application& GetInstance() { return *instance; }

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);

		inline Window& GetWindow() { return *window; }

	private:
		Application(const Application&);
		Application& operator=(Application&) {}

		
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		static Application* instance;
		bool isRunning;
		bool minimized;
		float lastFrameDt;
		
		std::unique_ptr<Window> window;

		LayerStack layerStack;
		ImGuiLayer* imguiLayer;

	};

	Application* CreateApplication();

}