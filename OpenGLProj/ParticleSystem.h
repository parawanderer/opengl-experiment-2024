#ifndef PARTICLESYSTEM_MINE_H
#define PARTICLESYSTEM_MINE_H
#include <vector>
#include <glm/vec3.hpp>

#include "Particle.h"
#include "Quad.h"
#include "Shader.h"
#include "WorldTimeManager.h"

/**
 * \brief Particles implementation based on https://learnopengl.com/In-Practice/2D-Game/Particles
 */
class ParticleSystem
{
public:
	ParticleSystem(
		WorldTimeManager* time, 
		const std::string& particleTexturePath, 
		const glm::vec3& particlesCenter, 
		const glm::vec3& particlesVelocity,
		float particleLifetime,
		int particleCount,
		const glm::vec2& particleSize
	);

	void onNewFrame();

	void draw(Shader& particleShader, const glm::mat4& view, const glm::vec3& cameraPos);

	void setCenterPosition(const glm::vec3& position);
	void setVelocity(const glm::vec3& velocity);

private:
	WorldTimeManager* _time;
	unsigned int _nrParticles = 500;
	std::vector<Particle> _particles;
	float _particleLifetime;

	Quad _quad;

	glm::vec3 _centerPosition;
	glm::vec3 _velocity;
	glm::vec2 _particleSize;

	unsigned int _lastUsedParticle = 0;
	unsigned int _textureId;


	/**
	 * \return Find first particle that is dead and return its index
	 */
	unsigned int findUnusedParticle();

	void respawnParticle(Particle& particle, const glm::vec3& particleCenterPosition, const glm::vec3& particleVelocity, const glm::vec3& offset);
	void sortParticles(const glm::vec3& cameraPos);
};

#endif