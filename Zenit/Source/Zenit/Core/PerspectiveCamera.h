#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

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
		inline const glm::vec3& GetForward() { return glm::cross(up, right); }

	private:
		bool HandleMovement(TimeStep ts);
		void ReCalculateMatrices();

	private:
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 target;
		glm::vec3 right;
		glm::vec3 up;

		glm::quat rotation;

		glm::mat4 view;
		glm::mat4 projection;

		float fovY;
	};

}