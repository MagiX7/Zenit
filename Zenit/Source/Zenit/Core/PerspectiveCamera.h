#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Zenit {

	class TimeStep;

	class PerspectiveCamera
	{
	public:
		// FOV Y in degrees.
		PerspectiveCamera(glm::vec3 pos, glm::vec3 target, float yFov);
		void Update(TimeStep ts);

		inline const glm::mat4x4& GetView() { return view; }
		inline const glm::mat4x4& GetProjection() { return projection; }

		inline const glm::vec3& GetPosition() { return position; }
		inline const glm::vec3& GetDirection() { return direction; }
		inline const glm::vec3& GetUp() { return up; }
		inline const glm::vec3& GetRight() { return right; }

	private:
		bool HandleMovement(TimeStep ts);
		void ReCalculateMatrices();

	private:
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 target;
		glm::vec3 right;
		glm::vec3 up;

		glm::mat4x4 view;
		glm::mat4x4 projection;

		float fovY;
	};

}