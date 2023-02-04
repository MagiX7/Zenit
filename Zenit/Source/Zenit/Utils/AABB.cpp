#include "AABB.h"
#include "Zenit/Renderer/VertexBuffer.h"

#include <glm/glm.hpp>

namespace Zenit {
	
	AABB::AABB() : membersChanged(false)
	{
		min = { FLT_MAX, FLT_MAX, FLT_MAX };
		max = { FLT_MIN, FLT_MIN, FLT_MIN };
		
		vertexBuffer = std::make_shared<VertexBuffer>();
		vertexBuffer->SetLayout({
			{ShaderDataType::VEC3F, "position"},
		});
		
	}

	AABB::AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max), membersChanged(false)
	{
		
	}

	AABB::~AABB()
	{
	}

	void AABB::Enclose(const glm::vec3& point)
	{
		min = glm::min(min, point);
		max = glm::max(max, point);
		
		//if (newMin != min)
		//{
		//	min = newMin;
		//	//membersChanged = true;
		//}
		//if (newMax != max)
		//{
		//	max = newMax;
		//	//membersChanged = true;
		//}

	}

	void AABB::Enclose(const std::vector<glm::vec3>& points)
	{
		for (size_t i = 0; i < points.size(); ++i)
			Enclose(points[i]);
	}

	void AABB::Enclose(const AABB& other)
	{
		Enclose(other.min);
		Enclose(other.max);
	}

	void AABB::RefreshData()
	{
		SetVertexBufferData();
	}

	void AABB::Transform(const glm::mat4& rotation, float angle, const glm::vec3& axis)
	{
		const glm::mat4 translatedMin = glm::rotate(rotation, angle, axis);
		min = translatedMin * glm::vec4(min, 1);

		const glm::mat4 translatedMax = glm::rotate(rotation, angle, axis);
		max = translatedMax * glm::vec4(max, 1);
	}

	std::shared_ptr<VertexBuffer> AABB::GetVertexBuffer()
	{
		return vertexBuffer;
	}

	void AABB::SetVertexBufferData()
	{
		float data[] =
		{
			//min[0],
			max[0], min[1], min[2],
			max[0], max[1], min[2],
			min[0], max[1], min[2],

			min[0], min[1], max[2],
			max[0], min[1], max[2],
			//max[0],
			min[0], max[1], max[2],
			//min[0],

			min[0], min[1], max[2],
			max[0], min[1], min[2],
			max[0], min[1], max[2],
			
			max[0], max[1], min[2],
			//max[0],
			min[0], max[1], min[2],
			min[0], max[1], max[2],
		};

		// TODO: Shader does not receive the positions
		vertexBuffer->SetData(data, sizeof(float) * 3 * 3 * 4);
	}

}