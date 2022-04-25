#pragma once

#include <memory>

namespace Zenit {

	class Window;

	class Application
	{
	public:
		Application();
		~Application();

		void Run();

	private:
		void Start();
		void Update();
		void Draw();
		void CleanUp();

	private:
		Window* window;
	};


}