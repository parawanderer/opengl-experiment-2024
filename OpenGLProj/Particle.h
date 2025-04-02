#ifndef PARTICLE_MINE_H
#define PARTICLE_MINE_H
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

/**
 * \brief Data container for a particle
 */
struct Particle
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 velocity = glm::vec3(0.0f);
	glm::vec4 color = glm::vec4(1.0f);
	float life = 0.0f;
	float rotationRadians = 0.0f;
	bool flag = false;
};

#endif