#pragma once

#include "Zenit/Renderer/Frustum.h"

#include <glm/glm.hpp>

#include <array>

namespace Zenit {

    class AABB
    {
    public:
        AABB();
        ~AABB();

        /// Extend the bounding box on all sides by \p val.
        void Extend(float val);

        /// Expand the AABB to include point \p p.
        void Extend(const glm::vec3& p);

        /// Expand the AABB to include a sphere centered at \p center and of radius \p
        /// radius.
        /// \param[in]  center Center of sphere.
        /// \param[in]  radius Radius of sphere.
        void Extend(const glm::vec3& center, float radius);

        /// Expand the AABB to encompass the given \p aabb.
        void Extend(const AABB& aabb);

        /// Translates AABB by vector \p v.
        void Translate(const glm::vec3& v);

        /// Scale the AABB by \p scale, centered around \p origin.
        /// \param[in]  scale  3D vector specifying scale along each axis.
        /// \param[in]  origin Origin of scaling operation. Most useful origin would
        ///                    be the center of the AABB.
        void Scale(const glm::vec3& scale, const glm::vec3& origin);

        /// Retrieves the center of the AABB.
        glm::vec3 GetCenter() const;

        /// Retrieves the diagonal vector (computed as mMax - mMin).
        /// If the AABB is NULL, then a vector of all zeros is returned.
        glm::vec3 GetDiagonal() const;

        /// Retrieves the longest edge.
        /// If the AABB is NULL, then 0 is returned.
        float GetLongestEdge() const;

        /// Retrieves the shortest edge.
        /// If the AABB is NULL, then 0 is returned.
        float GetShortestEdge() const;

        /// Retrieves the AABB's minimum point.
        glm::vec3 GetMin() const { return min; }

        /// Retrieves the AABB's maximum point.
        glm::vec3 GetMax() const { return max; }

        /// Returns true if AABBs share a face overlap.
        /// \xxx Untested -- This function is not represented in our unit tests.
        bool Overlaps(const AABB& bb) const;


        bool IsInsideFrustum(const Frustum& camFrustum) const;
        bool IsOnOrForwardPlane(const Frustum::Plane& plane) const;

        std::array<glm::vec3, 8> GetVertices() const;


    private:

        glm::vec3 min;
        glm::vec3 max;
        glm::vec3 extents;
    };

}
