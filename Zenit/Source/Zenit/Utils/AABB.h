#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <memory>

namespace Zenit {

	class VertexBuffer;

	class AABB
	{
	public:
		AABB();
		AABB(glm::vec3 min, glm::vec3 max);
		virtual ~AABB();

		void Enclose(const glm::vec3& point);
		void Enclose(const std::vector<glm::vec3>& points);
		void Enclose(const AABB& other);
		void RefreshData();
		
		void Transform(const glm::mat4& transform, float angle, const glm::vec3& axis);

		inline const glm::vec3& GetMin() const { return min; }
		inline const glm::vec3& GetMax() const { return max; }
		inline const glm::vec3& GetCenter() const { return (min + max) * 0.5f; }

		std::shared_ptr<VertexBuffer> GetVertexBuffer();

	private:
		void SetVertexBufferData();

	private:
		glm::vec3 min;
		glm::vec3 max;

		std::shared_ptr<VertexBuffer> vertexBuffer;
		bool membersChanged;
	};

}