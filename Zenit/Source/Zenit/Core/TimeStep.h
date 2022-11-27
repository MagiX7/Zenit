#pragma once

namespace Zenit {

	class TimeStep
	{
	public:
		TimeStep(float time = 0.0f) : time(time) {};

		float GetSeconds() { return time; }
		float GetMilliseconds() { return (time * 1000); }
		
		operator float() { return time; }

	private:
		float time;
	};

}