#pragma once

#include <memory>

namespace Zenit {

	class Window;

	class Application
	{
	public:
		Application();
		virtual ~Application();	

		void Run();

	private:
		void Start();
		void Update();
		void Draw();
		void CleanUp();

	private:
		static Application* instance;
		
		Window* window;

	};

	Application* CreateApplication();

}