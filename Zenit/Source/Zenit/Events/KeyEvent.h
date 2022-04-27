#pragma once

#include "Event.h"

namespace Zenit {

	class KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return keycode; };

		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT)
		
	protected:
		KeyEvent(int keycode) : keycode(keycode) {}

		int keycode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), repeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return repeatCount; }

		EVENT_CLASS_TYPE(KEY_PRESSED)

	private:
		int repeatCount;
	};


	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

		EVENT_CLASS_TYPE(KEY_PRESSED)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

		EVENT_CLASS_TYPE(KEY_TYPED)

	};

}

