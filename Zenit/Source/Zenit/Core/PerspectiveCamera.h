#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Zenit {

	class TimeStep;

	class PerspectiveCamera
	{
	public:
		// FOV Y in degrees.
		PerspectiveCamera(glm::vec3 pos, glm::vec3 target, float yFov, float aspectRatio);
		~PerspectiveCamera();
		void Update(TimeStep ts);

		void Scroll(TimeStep ts);
		void UpdateFovAndAspectRatio(float width, float height);
		void SetApsectRatio(float value);

		inline const glm::mat4x4& GetView() const { return view; }
		inline const glm::mat4x4& GetProjection() const { return projection; }

		inline const glm::vec3& GetPosition() const { return position; }
		inline const glm::vec3& GetDirection()const { return direction; }
		inline const glm::vec3& GetUp() const { return up; }
		inline const glm::vec3& GetRight() const { return right; }
		inline const glm::vec3& GetForward() const { return forward; }

		inline float GetFovX() const { return fovX; }
		inline float GetFovY() const { return fovY; }

	private:
		bool HandleInput(TimeStep ts);
		bool HandleMovement(TimeStep ts);
		void RecalculateMatrices();

	private:
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 target;
		glm::vec3 right;
		glm::vec3 up;
		glm::vec3 forward;

		glm::vec3 rotation;

		glm::mat4 view;
		glm::mat4 projection;

		float fovY;
		float fovX;
		float aspectRatio = 0;
	};

}