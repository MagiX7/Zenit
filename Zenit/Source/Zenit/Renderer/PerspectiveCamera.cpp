#include "Zenit/Core/Application.h"
#include "Zenit/Core/Window.h"
#include "Zenit/Core/Input.h"

#include "PerspectiveCamera.h"
#include "Zenit/Core/TimeStep.h"

#include "Zenit/Utils/AABB.h"

#include "Zenit/Core/Log.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <algorithm>

namespace Zenit {

	PerspectiveCamera::PerspectiveCamera(glm::vec3 pos, glm::vec3 target)
		: position(pos)
	{
		this->target = target;
		RecalculateMatrices();
	}

	void PerspectiveCamera::Scroll(TimeStep ts)
	{
		if (float scroll = Input::GetInstance()->GetMouseScrollDy())
		{
			position += forward * scroll * float(ts) * 5.0f;
			RecalculateMatrices();
		}
	}

	void PerspectiveCamera::SetPerspective(float verticalFov, float nearClip, float farClip)
	{
		projectionType = ProjectionType::PERSPECTIVE;

		yFov = verticalFov;
		perspectiveNear = nearClip;
		perspectiveFar = farClip;

		RecalculateMatrices();
	}

	void PerspectiveCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		projectionType = ProjectionType::ORTHOGRAPHIC;

		orthoSize = size;
		orthoNear = nearClip;
		orthoFar = farClip;

		RecalculateMatrices();
	}

	void PerspectiveCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		aspectRatio = (float)width / (float)height;
		RecalculateMatrices();
	}

	void PerspectiveCamera::RecalculateMatrices()
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
		view = glm::inverse(transform);
		
		if (projectionType == ProjectionType::PERSPECTIVE)
		{
			projection = glm::perspective(glm::radians(yFov), aspectRatio, perspectiveNear, perspectiveFar);

			direction = glm::normalize(position - target);
			right = glm::normalize(glm::cross({ 0,1,0 }, direction));
			this->up = glm::cross(direction, right);

			forward = glm::normalize(glm::cross(up, right));


		}
		else if (projectionType == ProjectionType::ORTHOGRAPHIC)
		{
			float left = -orthoSize * aspectRatio * 0.5f;
			float right = orthoSize * aspectRatio * 0.5f;
			float bottom = -orthoSize * 0.5f;
			float top = orthoSize * 0.5f;

			projection = glm::ortho(left, right, bottom, top, orthoNear, orthoFar);
		}

	}

}
