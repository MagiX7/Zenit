#pragma once

namespace Zenit {

	class TimeStep
	{
	public:
		TimeStep(float time = 0.0f) : dt(time) {};

		float GetSeconds() { return dt; }
		float GetMilliseconds() { return (dt * 1000); }
		
		operator float() { return dt; }

	private:
		float dt;
	};

}