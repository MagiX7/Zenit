#include "Application.h"
#include "Window.h"
#include "Input.h"

#include "PerspectiveCamera.h"
#include "TimeStep.h"

#include "Log.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <algorithm>

namespace Zenit {

	PerspectiveCamera::PerspectiveCamera(glm::vec3 pos, glm::vec3 target, float yFov, float aspectRatio)
		: position(pos), target(target), fovY(yFov), aspectRatio(aspectRatio)
	{
		direction = glm::normalize(position - target);
		right = glm::normalize(glm::cross({ 0,1,0 }, direction));
		this->up = glm::cross(direction, right);

		forward = glm::normalize(glm::cross(up, right));
		rotation = glm::vec3(0, 0, 0);

		fovX = glm::atan(glm::tan(fovY * 0.5) * 1280 / 720);
		
		RecalculateMatrices();
	}

	PerspectiveCamera::~PerspectiveCamera()
	{

	}

	void PerspectiveCamera::Update(TimeStep ts)
	{
		//if (HandleInput(ts))
		//{
		//	RecalculateMatrices();
		//}
	}

	void PerspectiveCamera::Scroll(TimeStep ts)
	{
		if (float dy = Input::GetInstance()->GetMouseScrolDy())
		{
			position += dy * ts * 10 * forward;
			RecalculateMatrices();
		}
		/*if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_MIDDLE))
		{
			position -= Input::GetInstance()->GetMouseMotionX() * 1.0f * ts * right;
			position -= Input::GetInstance()->GetMouseMotionY() * 1.0f * ts * up;
		}*/

	}

	void PerspectiveCamera::UpdateFovAndAspectRatio(float width, float height)
	{
		aspectRatio = width / height;
		fovY = 2 * glm::atan((glm::tan(fovX / 2)) * 1 / aspectRatio);
		fovX = 2 * glm::atan(glm::tan(fovY * 0.5) * aspectRatio);

		projection = glm::perspective(glm::radians(fovY), aspectRatio, 0.01f, 100.0f);
	}

	void PerspectiveCamera::SetApsectRatio(float value)
	{
		aspectRatio = value;
		projection = glm::perspective(glm::radians(fovY), aspectRatio, 0.01f, 100.0f);
	}

	bool PerspectiveCamera::HandleInput(TimeStep ts)
	{
		bool needToRecalculate = false;
		//if (float dy = Input::GetInstance()->GetMouseScrolDy())
		//{
		//	position += Input::GetInstance()->GetMouseScrolDy() * ts * forward;
		//	needToRecalculate = true;
		//}
		//if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_MIDDLE))
		//{
		//	position -= Input::GetInstance()->GetMouseMotionX() * 1.0f * ts * right;
		//	position -= Input::GetInstance()->GetMouseMotionY() * 1.0f * ts * up;
		//	needToRecalculate = true;
		//}

		return needToRecalculate;
	}

	bool PerspectiveCamera::HandleMovement(TimeStep ts)
	{
		bool needToRecalculate = false;

		if (Input::GetInstance()->IsKeyPressed(KEY_W))
		{
			glm::vec3 f = GetForward();
			position += 0.5f * ts * f;
			needToRecalculate = true;
		}
		if (Input::GetInstance()->IsKeyPressed(KEY_S))
		{
			glm::vec3 f = GetForward();
			position -= 0.5f * ts * f;
			needToRecalculate = true;
		}
		if (Input::GetInstance()->IsKeyPressed(KEY_D))
		{
			position += 0.5f * ts * glm::normalize(glm::cross(direction, up));
			needToRecalculate = true;
		}
		if (Input::GetInstance()->IsKeyPressed(KEY_A))
		{
			position -= 0.5f * ts * glm::normalize(glm::cross(direction, up));
			needToRecalculate = true;
		}
		if (Input::GetInstance()->IsKeyPressed(KEY_E))
		{
			position += 0.5f * ts * up;
			needToRecalculate = true;
		}
		if (Input::GetInstance()->IsKeyPressed(KEY_Q))
		{
			position -= 0.5f * ts * up;
			needToRecalculate = true;
		}

		return needToRecalculate;
	}

	void PerspectiveCamera::RecalculateMatrices()
	{
		// Maybe not a good idea using rotation as vec3.
		// Should switch to quat
		// Try with glm::rotate or glm::rotation;

		const glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
		view = glm::inverse(transform);

		projection = glm::perspective(glm::radians(fovY), aspectRatio, 0.01f, 100.0f);
	}

}
