#include "znpch.h"
#include "Application.h"
#include "Window.h"
#include "Input.h"

#include "Log.h"

#include "Zenit/Events/Event.h"
#include "Zenit/Events/ApplicationEvent.h"

#include <glad/glad.h>

namespace Zenit {

	Application* Application::instance = nullptr;

	Application::Application()
	{
		instance = this;
		
		Log::Init();
		ZN_CORE_INFO("[INFO]: Logging system initialized");
		
		window = std::make_unique<Window>(1280, 720);
		window->SetEventCallback(ZN_BIND_EVENT_FN(Application::OnEvent));

		imguiLayer = new ImGuiLayer();
		PushOverlay(imguiLayer);

		isRunning = true;
		minimized = false;
	}

	Application::~Application()
	{
		delete instance;
	}

	void Application::Run()
	{
		while (isRunning)
		{
			// Ideal would be having Renderer::Clear();
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.1, 0.1, 0.1, 1.0);

			float t = glfwGetTime();
			TimeStep dt = t - lastFrameDt;
			lastFrameDt = t;

			if (!minimized)
				for (auto& l : layerStack)
					l->OnUpdate(lastFrameDt);

			imguiLayer->Begin();
			for (auto& l : layerStack)
				l->OnImGuiRender();
			imguiLayer->End();
			
			window->Update();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		layerStack.PushLayer(layer);
	}

	void Application::PopLayer(Layer* layer)
	{
		layerStack.PopLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		layerStack.PushOverlay(overlay);
	}

	void Application::PopOverlay(Layer* overlay)
	{
		layerStack.PopOverlay(overlay);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
		dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));
		
		for (auto it = layerStack.end(); it != layerStack.begin();)
		{
			// --it here and not in the loop because if the event has been handled we dont want to propagate it to the under layers
			(*--it)->OnEvent(e);
			if (e.IsHandled())
				break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		isRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			minimized = true;
			return false;
		}

		minimized = false;
	}
}