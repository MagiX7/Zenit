#include "Frustum.h"

namespace Zenit {

	Frustum::Frustum(PerspectiveCamera& cam)
	{
        UpdateFrustum(cam);
	}

    void Frustum::UpdateFrustum(PerspectiveCamera& cam)
    {
        float zFar = cam.GetPerspectiveFarClip();
        float zNear = cam.GetPerspectiveNearClip();
        float fovY = cam.GetVerticalFov();
        float aspect = cam.GetAspectRatio();
        glm::vec3 front = cam.GetForward();

        const float halfVSide = zFar * tanf(fovY * .5f);
        const float halfHSide = halfVSide * aspect;
        const glm::vec3 frontMultFar = zFar * front;

        glm::vec3 position = cam.GetPosition();

        nearFace = { position + zNear * front, front };
        farFace = { position + frontMultFar, -front };
        rightFace = { position, glm::cross(frontMultFar - cam.GetRight() * halfHSide, cam.GetUp()) };
        leftFace = { position, glm::cross(cam.GetUp(),frontMultFar + cam.GetRight() * halfHSide) };
        topFace = { position, glm::cross(cam.GetRight(), frontMultFar - cam.GetUp() * halfVSide) };
        bottomFace = { position, glm::cross(frontMultFar + cam.GetUp() * halfVSide, cam.GetRight()) };
    }

}