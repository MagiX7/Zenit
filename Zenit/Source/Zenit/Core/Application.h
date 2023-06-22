#pragma once

#include "Zenit/Events/ApplicationEvent.h"
#include "Zenit/ImGui/ImGuiLayer.h"
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

		void Terminate() { isRunning = false; }
		bool ExitRequested() { return shouldClose; }

		void PushLayer(EngineLayer* layer);
		void PopLayer(EngineLayer* layer);
		void PushOverlay(EngineLayer* overlay);
		void PopOverlay(EngineLayer* overlay);

		inline Window& GetWindow() { return *window; }
		inline float GetTimeStep() { return timestep; }
		float GetTotalExecutionTime();

	private:
		Application(const Application&);
		Application& operator=(Application&) {}
		
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		static Application* instance;
		bool isRunning;
		bool shouldClose;
		bool minimized;

		TimeStep timestep = 0;
		float lastFrameTime = 0;
		
		std::unique_ptr<Window> window;

		LayerStack layerStack;
		ImGuiLayer* imguiLayer;

	};

	Application* CreateApplication();

}