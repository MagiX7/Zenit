#pragma once

#include "Zenit/Renderer/PerspectiveCamera.h"

#include <glm/glm.hpp>

namespace Zenit {

	class Frustum
	{
	public:
		Frustum(PerspectiveCamera& cam);

		void UpdateFrustum(PerspectiveCamera& cam);

	public:
		struct Plane
		{
			// unit vector
			glm::vec3 normal = { 0.f, 1.f, 0.f };

			// distance from origin to the nearest point in the plane
			float distance = 0.f;

			Plane() {}
			Plane(const glm::vec3& p1, const glm::vec3& norm)
				: normal(glm::normalize(norm)),
				distance(glm::dot(normal, p1))
			{}


			float GetSignedDistanceToPlane(const glm::vec3& point) const
			{
				return glm::dot(normal, point) - distance;
			}
		};

	public:
		Plane topFace;
		Plane bottomFace;

		Plane rightFace;
		Plane leftFace;

		Plane farFace;
		Plane nearFace;

	};

}