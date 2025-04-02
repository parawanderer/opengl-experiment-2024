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
	virtual ~ParticleSystem() = default;

	ParticleSystem(
		WorldTimeManager* time, 
		const std::string& particleTexturePath, 
		const glm::vec3& particlesCenter,
		float particleLifetime,
		unsigned int particleCount,
		unsigned int particlesToSpawnEachFrame,
		const glm::vec2& particleSize
	);

	virtual void onNewFrame();

	virtual void draw(Shader& particleShader, const glm::mat4& view, const glm::vec3& cameraPos);

	void setCenterPosition(const glm::vec3& position);
	WorldTimeManager* getTimeManager() const;
	unsigned int getNumParticles() const;
	float getParticleLifetime() const;
	glm::vec3 getCurrentCenterPosition() const;
	glm::vec2 getParticleSize() const;

protected: // allow these to be overwritten in any future particle systems that try to achieve alternate effects

	/**
	 * Called on each frame to update the state of an individual particle at that given frame
	 */
	virtual void updateAliveParticle(Particle& p, const float deltaTime);

	/**
	 * Configure shader uniform variables for drawing the particle effect (global variables, not per-particle)
	 */
	virtual void setupShaderForDraw(Shader& particleShader, const glm::mat4& view);

	/**
	 * \brief	This is called every time a new particle is added to the system. This fills in the initial values of a
	 *			particle that will change using updateAliveParticle() over its lifetime.
	 *
	 * \param particle					This is the new particle that is being configured. Should be used to set initial property values into
	 * \param particleCenterPosition	This is the current center-position of the particle system
	 */
	void respawnParticle(Particle& particle, const glm::vec3& particleCenterPosition);

	/**
	 * \brief This is called when drawing a single particle during the draw call.
	 *	This is responsible for setting per-particle uniform variables for the shader as well as doing the draw call itself.
	 */
	void drawParticleOnAlive(const Particle& p, Shader& particleShader);

	/**
	 * \brief Draws a single particle. Assumes that before this is called the appropriate per-particle matrices were already set
	 */
	void drawQuadWithTexture();

private:
	WorldTimeManager* _time;
	unsigned int _nrParticles;
	unsigned int _particlesToSpawnEachFrame;
	std::vector<Particle> _particles;
	float _particleLifetime;

	Quad _quad;

	glm::vec3 _centerPosition;
	glm::vec2 _particleSize;

	unsigned int _lastUsedParticle = 0;
	unsigned int _textureId;

	/**
	 * \return Find first particle that is dead and return its index
	 */
	unsigned int findUnusedParticle();
	void sortParticles(const glm::vec3& cameraPos);
};

#endif