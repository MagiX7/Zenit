#pragma once

#include "Event.h"

namespace Zenit {

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(float width, float height) : width(width), height(height) {}

		EVENT_CLASS_TYPE(WINDOW_RESIZE);
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_APP)

		inline float GetWidth() { return width; }
		inline float GetHeight() { return height; }

	private:
		float width, height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {};

		EVENT_CLASS_TYPE(WINDOW_CLOSE);
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_APP)
	};

}