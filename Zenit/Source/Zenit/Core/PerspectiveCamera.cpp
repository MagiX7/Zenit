#include "PerspectiveCamera.h"
#include "Input.h"
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

		view = glm::lookAt(position, this->target, up);
		projection = glm::perspective(glm::radians(fovY), 1.0f, 0.5f, 1000.0f);
	}

	void PerspectiveCamera::Update(TimeStep ts)
	{
		if (HandleMovement(ts))
		{
			ReCalculateMatrices();
		}
	}

	bool PerspectiveCamera::HandleMovement(TimeStep ts)
	{
		bool needToRecalculate = false;

		if (Input::IsKeyPressed(KEY_W))
		{
			position.z += 0.5 * ts;
			needToRecalculate = true;
		}
		if (Input::IsKeyPressed(KEY_S))
		{
			position.z -= 0.5 * ts;
			needToRecalculate = true;
		}
		if (Input::IsKeyPressed(KEY_D))
		{
			position.x -= 0.5 * ts;
			needToRecalculate = true;
		}
		if (Input::IsKeyPressed(KEY_A))
		{
			position.x -= 0.5 * ts;
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

	void PerspectiveCamera::ReCalculateMatrices()
	{
		view = glm::lookAt(position, this->target, up);
	}

}