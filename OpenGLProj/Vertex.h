#ifndef VERTEX_MINE_H
#define VERTEX_MINE_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

#endif