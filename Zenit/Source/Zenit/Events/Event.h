#pragma once

#include "Zenit/Core/Defs.h"

namespace Zenit {

	enum class EventType
	{
		NONE = 0,
		WINDOW_CLOSE, WINDOW_RESIZE, WINDOW_FOCUS, WINDOW_LOST_FOCUS, WINDOW_MOVED,
		KEY_PRESSED, KEY_RELEASED, KEY_TYPED,
		MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOUSE_MOVE, MOUSE_SCROLLED
	};

	enum EventCategory
	{
		NONE = 0,
		EVENT_CATEGORY_APP = BIT(0),
		EVENT_CATEGORY_INPUT = BIT(1),
		EVENT_CATEGORY_KEYBOARD = BIT(2),
		EVENT_CATEGORY_MOUSE = BIT(3),
		EVENT_CATEGORY_MOUSEBUTTON = BIT(4),
	};


#define EVENT_CLASS_TYPE(t) static EventType GetStaticType() { return EventType::##t; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #t; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }


	class Event
	{
		friend class EventDispatcher;

	public:
		virtual ~Event() {}

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
		
		inline bool IsHandled() { return handled; }

	protected:
		// To not propagate the event to under layers
		bool handled = false;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& e) : event(e) {};

		template <typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (event.GetEventType() == T::GetStaticType())
			{
				event.handled |= func(static_cast<T&>(event));
				return true;
			}

			return false;
		}
		
	private:
		Event& event;
	};

	//inline std::ostream operator <<(std::ostream& os, const Event& e)
	//{
	//	return os << e.ToString();
	//}

}