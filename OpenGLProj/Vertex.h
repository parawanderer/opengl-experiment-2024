#ifndef VERTEX_MINE_H
#define VERTEX_MINE_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

/**
 * \brief Basic OpenGL vertex data container
 */
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

#endif