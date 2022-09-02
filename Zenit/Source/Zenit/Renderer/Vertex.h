#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normals;
	glm::vec2 texCoords;
	glm::vec3 tangents;
	glm::vec3 biTangents;
};