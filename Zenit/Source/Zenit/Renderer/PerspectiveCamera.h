#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Zenit {

	class TimeStep;
	class AABB;

	class PerspectiveCamera
	{
	public:
		enum class ProjectionType { PERSPECTIVE = 0, ORTHOGRAPHIC = 1, };

	public:
		PerspectiveCamera(glm::vec3 pos, glm::vec3 target);
		virtual ~PerspectiveCamera() {}

		void Scroll(TimeStep ts);

		void SetPerspective(float verticalFov, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetProjectionType(ProjectionType type) { projectionType = type; RecalculateMatrices(); }
		ProjectionType GetProjectionType() const { return projectionType; }

		const glm::mat4& GetProjectionMatrix() { return projection; }
		const glm::mat4& GetViewMatrix() { return view; }

		// Perspective Methods
		float GetPerspectiveNearClip() const { return perspectiveNear; }
		void SetPerspectiveNearClip(float value) { perspectiveNear = value; RecalculateMatrices(); }

		float GetPerspectiveFarClip() const { return perspectiveFar; }
		void SetPerspectiveFarClip(float value) { perspectiveFar = value; RecalculateMatrices(); }

		float GetVerticalFov() const { return yFov; }
		void SetVerticalFov(float value) { yFov = value; RecalculateMatrices(); }

		// Orthographic Methods
		float GetOrthographicFarClip() const { return orthoFar; }
		void SetOrthographicFarClip(float value) { orthoFar = value; RecalculateMatrices(); }

		float GetOrthographicNearClip() const { return orthoNear; }
		void SetOrthographicNearClip(float value) { orthoNear = value; RecalculateMatrices(); }

		float GetOrthographicSize() const { return orthoSize; }
		void SetOrthographicSize(float value) { orthoSize = value; RecalculateMatrices(); }

		float GetAspectRatio() const { return aspectRatio; }
		void SetAspectRatio(float value) { aspectRatio = value; RecalculateMatrices(); }


		glm::vec3& GetPosition() { return position; }
		void SetPosition(glm::vec3 value) { position = value; RecalculateMatrices(); }

		glm::vec3& GetRotation() { return rotation; }
		void SetRotation(glm::vec3 value) { rotation = value; RecalculateMatrices(); }

		inline const glm::vec3& GetDirection()const { return direction; }
		inline const glm::vec3& GetUp() const { return up; }
		void SetUp(const glm::vec3& newUp) { up = newUp; RecalculateMatrices(); }
		inline const glm::vec3& GetRight() const { return right; }
		void SetRight(const glm::vec3& newRight) { right = newRight; RecalculateMatrices(); }
		inline const glm::vec3& GetForward() const { return forward; }
		void SetForward(const glm::vec3& newForward) { forward = newForward; RecalculateMatrices(); }

	private:
		void RecalculateMatrices();

	private:
		ProjectionType projectionType = ProjectionType::PERSPECTIVE;

		glm::mat4 projection = glm::mat4(1.0);
		glm::mat4 view = glm::mat4(1.0);
		
		glm::vec3 position = glm::vec3(0);
		glm::vec3 rotation = glm::vec3(0);
		glm::vec3 direction = glm::vec3(0);
		glm::vec3 up = glm::vec3(0);
		glm::vec3 right = glm::vec3(0);
		glm::vec3 forward = glm::vec3(0);
		glm::vec3 target = glm::vec3(0);

		
		float yFov = 60.0f;
		float perspectiveNear = 0.01f;
		float perspectiveFar = 1000.0f;

		float orthoSize = 10.0f;
		float orthoNear = -1;
		float orthoFar = 1;

		float aspectRatio = 0.0f;

	};

}