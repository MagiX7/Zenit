#include "Zenit/Core/Application.h"
#include "Zenit/Core/Window.h"
#include "Zenit/Core/Input.h"

#include "Model.h"
#include "Zenit/Core/PerspectiveCamera.h"

#include "Zenit/Renderer/Shader.h"
#include "Zenit/Renderer/Texture2D.h"

#include "Zenit/Core/TimeStep.h"
#include "Zenit/Core/Log.h"

#include "Zenit/Renderer/Skybox.h"

#include <glm/glm.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/matrix_decompose.hpp>


namespace Zenit {

	Model::Model(std::string path) : path(path)
	{
		rotation = glm::quat(0, 0, 0, 1);
		transform = glm::translate(glm::mat4(1.0), { 0,0,0 }) * glm::toMat4(rotation);
	}

	Model::~Model()
	{
	}

	void Model::Update(TimeStep ts, float dx, float dy)
	{
		static bool firstInput = true;

		//float x = Input::GetInstance()->GetMouseX();
		//float y = Input::GetInstance()->GetMouseY();
		//
		//static float lastX = x;
		//static float lastY = y;
		//
		////float dx = (x - lastX) / Application::GetInstance().GetWindow().GetWidth();
		////float dy = (lastY - y) / Application::GetInstance().GetWindow().GetHeight();
		//float dx = Input::GetInstance()->GetMouseMotionX();
		//float dy = Input::GetInstance()->GetMouseMotionY();

		if (!Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
		{
			firstInput = true;
		}
		else if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
		{
			if (firstInput)
			{
				firstInput = false;
				return;
			}

			if (dy)
			{
				glm::vec3 right = glm::vec3(transform[0][0], transform[1][0], transform[2][0]);
				transform = glm::rotate(transform, -dy * ts, right);
			}
			if (dx)
			{
				glm::vec3 up = glm::vec3(transform[0][1], transform[1][1], transform[2][1]);
				transform = glm::rotate(transform, dx * ts, up);
			}
		}

		//lastX = x;
		//lastY = y;
	}

	void Model::Draw() const
	{
		//shader->Bind();
		//shader->SetUniformMatrix4f("view", camera.GetView());
		//shader->SetUniformMatrix4f("projection", camera.GetProjection());

		//shader->SetUniformMatrix4f("model", transform);
		//shader->SetUniformVec3f("camPos", camera.GetPosition());

		//diffuse->Bind();
		//shader->SetUniform1i("colorTexture", 0);

		//skybox->Bind(1);
		//shader->SetUniform1i("skybox", 1);

		//shader->SetUniform1f("skyboxIntensity", skybox->GetInstensity());
		//shader->SetUniform1i("skyboxReflectionEnabled", skybox->IsReflectionEnabled());

		for (auto& m : meshes)
		{
			m->Draw();
		}

		/*skybox->Unbind();
		diffuse->Unbind();
		shader->Unbind();*/
	}

	void Model::ResetRotation()
	{
		transform = glm::toMat4(glm::quat(0, 0, 0, 1));
	}
}