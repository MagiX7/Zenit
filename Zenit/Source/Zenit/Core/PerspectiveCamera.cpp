#include "Application.h"
#include "Window.h"
#include "Input.h"

#include "PerspectiveCamera.h"
#include "TimeStep.h"

#include "Log.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Zenit {

	PerspectiveCamera::PerspectiveCamera(glm::vec3 pos, glm::vec3 target, float yFov) : position(pos), target(target), fovY(yFov)
	{
		direction = glm::normalize(position - target);
		right = glm::normalize(glm::cross({ 0,1,0 }, direction));
		this->up = glm::cross(direction, right);

		rotation = glm::quat(0, 0, 0, 1);

		//view = glm::lookAt(position, this->target, up);
		//projection = glm::perspective(glm::radians(fovY), 1.0f, 0.5f, 1000.0f);
		RecalculateMatrices();
	}

	void PerspectiveCamera::Update(TimeStep ts)
	{
		if (HandleMovement(ts))
		{
			RecalculateMatrices();
		}
	}

	bool PerspectiveCamera::HandleMovement(TimeStep ts)
	{
		bool needToRecalculate = false;

		if (Input::IsKeyPressed(KEY_W))
		{
			//position.z -= 0.5 * ts;
			position -= 0.5f * ts * GetForward();
			needToRecalculate = true;
		}
		if (Input::IsKeyPressed(KEY_S))
		{
			//position.z += 0.5 * ts;
			position += 0.5f * ts * GetForward();
			needToRecalculate = true;
		}
		if (Input::IsKeyPressed(KEY_D))
		{
			//position.x -= 0.5 * ts;
			position += 0.5f * ts * glm::normalize(glm::cross(direction, up));
			needToRecalculate = true;
		}
		if (Input::IsKeyPressed(KEY_A))
		{
			//position.x -= 0.5 * ts;
			position -= 0.5f * ts * glm::normalize(glm::cross(direction, up));
			needToRecalculate = true;
		}
		if (Input::IsKeyPressed(KEY_E))
		{
			position.y += 0.5 * ts;
			needToRecalculate = true;
		}
		if (Input::IsKeyPressed(KEY_Q))
		{
			position.y -= 0.5 * ts;
			needToRecalculate = true;
		}

		return needToRecalculate;
	}

	void PerspectiveCamera::RecalculateMatrices()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::toMat4(rotation);
		view = glm::inverse(transform);

		float h = Application::GetInstance().GetWindow().GetHeight();
		float w = Application::GetInstance().GetWindow().GetWidth();
		projection = glm::perspective(glm::radians(fovY), w / h, 0.01f, 10000.0f);
	}
}