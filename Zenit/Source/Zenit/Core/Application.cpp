#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Zenit/Renderer/Renderer3D.h"

#include "Log.h"

#include "Zenit/Events/Event.h"
#include "Zenit/Events/ApplicationEvent.h"

#include <glad/glad.h>
#include <iostream>

namespace Zenit {

	Application* Application::instance = nullptr;

	Application::Application()
	{
		instance = this;
		
		Log::Init();
		ZN_CORE_INFO("[INFO] Logging system initialized");
		
		window = std::make_unique<Window>(1720, 920, "Zenit Editor");
		window->SetEventCallback(ZN_BIND_EVENT_FN(Application::OnEvent));

		Renderer3D::GetInstance()->Init();

		imguiLayer = new ImGuiLayer();
		PushOverlay(imguiLayer);

		isRunning = true;
		minimized = false;
	}

	Application::~Application()
	{
		Input::GetInstance()->Shutdown();
	}

	void Application::Run()
	{
		while (isRunning)
		{
			Renderer3D::GetInstance()->Clear({ 0.2f,0.2f,0.2f,1.0f });

			float t = glfwGetTime();
			timestep = t - lastFrameTime;
			lastFrameTime = t;

			if (!minimized)
				for (auto& l : layerStack)
					l->OnUpdate(timestep);

			imguiLayer->Begin();
			for (auto& layer : layerStack)
				layer->OnImGuiRender();
			imguiLayer->End();
			
			Input::GetInstance()->ResetScrollStats();
			if (shouldClose) shouldClose = false;

			window->SwapBuffers();
		}
	}

	void Application::PushLayer(EngineLayer* layer)
	{
		layerStack.PushLayer(layer);
	}

	void Application::PopLayer(EngineLayer* layer)
	{
		layerStack.PopLayer(layer);
	}

	void Application::PushOverlay(EngineLayer* overlay)
	{
		layerStack.PushOverlay(overlay);
	}

	void Application::PopOverlay(EngineLayer* overlay)
	{
		layerStack.PopOverlay(overlay);
	}

	float Application::GetTotalExecutionTime()
	{
		return glfwGetTime();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(ZN_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(ZN_BIND_EVENT_FN(Application::OnWindowResize));
		Input::GetInstance()->OnEvent(e);
		
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
		//isRunning = false;
		shouldClose = true;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			minimized = true;
			return false;
		}
		
		Renderer3D::GetInstance()->OnResize(e.GetWidth(), e.GetHeight());
		
		minimized = false;
	}
}