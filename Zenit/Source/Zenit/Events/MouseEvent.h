#pragma once

#include "Event.h"

namespace Zenit {

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y) : mouseX(x), mouseY(y) {}

		inline float GetX() { return mouseX; }
		inline float GetY() { return mouseY; }
		
		EVENT_CLASS_TYPE(MOUSE_MOVE)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE| EVENT_CATEGORY_INPUT)

	private:
		float mouseX;
		float mouseY;
	};


	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset) : offsetX(xOffset), offsetY(yOffset) {};

		EVENT_CLASS_TYPE(MOUSE_SCROLLED);
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT);

		inline float GetOffsetX() { return offsetX; }
		inline float GetOffsetY() { return offsetY; }

	private:
		float offsetX;
		float offsetY;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline int GetButton() { return button; }

		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)

	protected:
		MouseButtonEvent(int button) : button(button) {};

		int button;
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		EVENT_CLASS_TYPE(MOUSE_BUTTON_RELEASED)
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		EVENT_CLASS_TYPE(MOUSE_BUTTON_PRESSED)
	};
}