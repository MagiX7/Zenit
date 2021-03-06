#pragma once

#include "KeyCodes.h"
#include "Zenit/Events/Event.h"
#include "Zenit/Events/MouseEvent.h"

#include <utility>

namespace Zenit {

	class Input
	{
	public:
		static Input* GetInstance();

		bool IsKeyPressed(int keycode);
		bool IsMouseButtonPressed(int button);

		float GetMouseX();
		float GetMouseY();
		float GetMouseMotionX();
		float GetMouseMotionY();
		std::pair<float, float> GetMousePosition();
		inline float GetMouseScrolDx() { return deltaXScroll; }
		inline float GetMouseScrolDy() { return deltaYScroll; }

		void OnEvent(Event& e);

		void ResetScrollStats();

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);

	private:
		static Input* instance;

		float deltaXScroll;
		float deltaYScroll;

		float lastMousePosX = 0;
		float lastMousePosY = 0;
		double x, y;
	};	

}