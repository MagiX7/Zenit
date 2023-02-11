#include "AABB.h"
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Zenit {

    AABB::AABB()
    {
        min = glm::vec3(std::numeric_limits<float>().max());
        max = glm::vec3(std::numeric_limits<float>().min());
        extents = glm::vec3(0);
    }

    AABB::~AABB()
    {
    }

    bool AABB::IsInsideFrustum(const Frustum& camFrustum) const
    {
        // Get global scale is computed by doing the magnitude of
            //X, Y and Z model matrix's column.

        //glm::vec3 scale;
        //glm::quat rotation;
        //glm::vec3 translation;
        //glm::vec3 skew;
        //glm::vec4 perspective;
        //glm::decompose(transform, scale, rotation, translation, skew, perspective);


        ////Get our global center with process it with the global model matrix of our transform
        //const glm::vec3 globalCenter{ transform * glm::vec4(GetCenter(), 1.f)};

        ////To wrap correctly our shape, we need the maximum scale scalar.
        //const float maxScale = glm::max(glm::max(scale.x, scale.y), scale.z);

        ////Max scale is assuming for the diameter. So, we need the half to apply it to our radius
        //Sphere globalSphere(globalCenter, radius * (maxScale * 0.5f));

        //Check Firstly the result that have the most chance
        //to faillure to avoid to call all functions.
        return (IsOnOrForwardPlane(camFrustum.leftFace) &&
            IsOnOrForwardPlane(camFrustum.rightFace) &&
            IsOnOrForwardPlane(camFrustum.farFace) &&
            IsOnOrForwardPlane(camFrustum.nearFace) &&
            IsOnOrForwardPlane(camFrustum.topFace) &&
            IsOnOrForwardPlane(camFrustum.bottomFace));
    }

    bool AABB::IsOnOrForwardPlane(const Frustum::Plane& plane) const
    {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const float r = extents.x * std::abs(plane.normal.x) +
            extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);

        return -r <= plane.GetSignedDistanceToPlane(GetCenter());
    }

    std::array<glm::vec3, 8> AABB::GetVertices() const
    {
        auto center = GetCenter();
        std::array<glm::vec3, 8> vertice;
        vertice[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z };
        vertice[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
        vertice[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
        vertice[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
        vertice[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
        vertice[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
        vertice[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
        vertice[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
        return vertice;
    }

    void AABB::Extend(float val)
    {
        //if (!IsNull())
        {
            min -= glm::vec3(val);
            max += glm::vec3(val);
        }
    }

    void AABB::Extend(const glm::vec3& p)
    {
        //if (!IsNull())
        {
            min = glm::min(p, min);
            max = glm::max(p, max);

            auto center = GetCenter();
            extents = { max.x - center.x, max.y - center.y, max.z - center.z };
        }
        //else
        //{
        //    min = p;
        //    max = p;
        //}
    }

    void AABB::Extend(const glm::vec3& p, float radius)
    {
        glm::vec3 r(radius);
        //if (!IsNull())
        {
            min = glm::min(p - r, min);
            max = glm::max(p + r, max);
        }
        //else
        //{
        //    min = p - r;
            max = p + r;
        //}
    }

    void AABB::Extend(const AABB& aabb)
    {
        //if (!aabb.IsNull())
        {
            Extend(aabb.min);
            Extend(aabb.max);
        }
    }


    glm::vec3 AABB::GetDiagonal() const
    {
        //if (!IsNull())
            return max - min;
        //else
        //    return glm::vec3(0);
    }

    float AABB::GetLongestEdge() const
    {
        return glm::compMax(GetDiagonal());
    }

    float AABB::GetShortestEdge() const
    {
        return glm::compMin(GetDiagonal());
    }

    glm::vec3 AABB::GetCenter() const
    {
        //if (!IsNull())
        {
            //glm::vec3 d = GetDiagonal();
            //return min + (d * float(0.5));
            return (max + min) * 0.5f;
        }
        //else
        //{
        //    return glm::vec3(0.0);
        //}
    }

    void AABB::Translate(const glm::vec3& v)
    {
        //if (!IsNull())
        {
            min += v;
            max += v;
        }
    }

    void AABB::Scale(const glm::vec3& s, const glm::vec3& o)
    {
        //if (!IsNull())
        {
            min -= o;
            max -= o;

            min *= s;
            max *= s;

            min += o;
            max += o;
        }
    }

    bool AABB::Overlaps(const AABB& bb) const
    {
        //if (IsNull() || bb.IsNull())
        //    return false;

        if (bb.min.x > max.x || bb.max.x < min.x)
            return false;
        else if (bb.min.y > max.y || bb.max.y < min.y)
            return false;
        else if (bb.min.z > max.z || bb.max.z < min.z)
            return false;

        return true;
    }



}