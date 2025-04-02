#include "ParticleSystem.h"

#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include "Colors.h"
#include "ConfigConstants.h"
#include "ErrorUtils.h"
#include "FileUtils.h"
#include "WorldMathUtils.h"


// https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
// sidenote that the particles honestly look bad. I looked around for how better looks are accomplished and found this:
// https://www.youtube.com/watch?v=Qj_tK_mdRcA, https://www.youtube.com/watch?v=4QOcCGI6xOU
// Which was an interesting bit of detail that also uses some of the techniques I read about/applied for my fixed-width "outline" solution.
// It does however seem way out of scope to implement this, so I guess I have to stick with these "ugly" particles.

// this entire thing could probably also be improved by this: https://learnopengl.com/Advanced-OpenGL/Instancing
ParticleSystem::ParticleSystem(
	WorldTimeManager* time, 
	const std::string& particleTexturePath, 
	const glm::vec3& particlesCenter,
	float particleLifetime,
	unsigned int particleCount,
	unsigned int particlesToSpawnEachFrame,
	const glm::vec2& particleSize)
:
_time(time),
_nrParticles(particleCount),
_particlesToSpawnEachFrame(particlesToSpawnEachFrame),
_particleLifetime(particleLifetime),
_centerPosition(particlesCenter),
_particleSize(particleSize)
{
	// particle texture
	this->_textureId = loadSRGBColorSpaceTexture(particleTexturePath.c_str(), PROJ_CURRENT_DIR, GL_TEXTURE0);
	
	for (unsigned int i = 0; i < this->_nrParticles; ++i)
	{
		this->_particles.push_back(Particle()); // defaults
	}
}

void ParticleSystem::onNewFrame()
{
	const float deltaTime = this->_time->getDeltaTime();

	// add new particles
	for (unsigned int i = 0; i < this->_particlesToSpawnEachFrame; ++i)
	{
		const int unusedParticleIndex = this->findUnusedParticle();
		this->respawnParticle(this->_particles[unusedParticleIndex], this->_centerPosition);
	}
	// update all particles
	for (unsigned int i = 0; i < this->_nrParticles; ++i)
	{
		Particle& p = this->_particles[i];
		p.life -= deltaTime; // reduce life

		if (p.life > 0.0f) // particle is alive, thus update it:
		{
			this->updateAliveParticle(p, deltaTime);
		}
	}
}

void ParticleSystem::draw(Shader& particleShader, const glm::mat4& view, const glm::vec3& cameraPos)
{
	//this->sortParticles(cameraPos);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);    // <- "glowy" blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);

	
	particleShader.use();
	this->setupShaderForDraw(particleShader, view);

	// so this ordering ensures the "oldest" particles are drawn first,
	// such that when standing "in front of" the effect you don't see weird opacity layering artifacts
	// for my use case this is good enough since you would typically only see the sand worm creature
	// emitting these particles from the front
	// but the effect breaks down starting from a side view and is completely weird from a back view.
	// sorting a lot of particles is also very slow and laggy
	// and a more proper fix would be something like this but the effort is not worth it to me
	// https://youtu.be/4QOcCGI6xOU?si=h6oS3hDgom3dvMCo&t=317
	// as I don't really like the particle effect visuals even with depth testing disabled.
	// It's just not very good-looking in the first place.
	for (unsigned int i = this->_lastUsedParticle; i < this->_nrParticles; ++i) // oldest first
	{
		Particle& p = this->_particles[i];
		if (p.life > 0.0f) this->drawParticleOnAlive(p, particleShader);
	}

	for (unsigned int i = 0; i < this->_lastUsedParticle; ++i) // then newest
	{
		Particle& p = this->_particles[i];
		if (p.life > 0.0f) this->drawParticleOnAlive(p, particleShader);
	}

	glDisable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::setCenterPosition(const glm::vec3& position)
{
	this->_centerPosition = position;
}

unsigned ParticleSystem::findUnusedParticle()
{
	// search from last used particle, this will usually return almost instantly
	for (unsigned int i = this->_lastUsedParticle; i < this->_nrParticles; ++i)
	{
		if (this->_particles[i].life <= 0.0f)
		{
			this->_lastUsedParticle = i;
			return i;
		}
	}
	// otherwise do linear search (searches the remainder of the list before this lastUsedParticle if above failed)
	for (unsigned int i = 0; i < this->_lastUsedParticle; ++i)
	{
		if (this->_particles[i].life <= 0.0f)
		{
			this->_lastUsedParticle = i;
			return i;
		}
	}
	// override first particle if all others are alive
	// **NOTE:**
	// if this case is reached it means our particles are alive for too long,
	// and we should consider either spawning less particles per frame and/or reserving a larger number of particles!
	this->_lastUsedParticle = 0;
	std::cout << "Particle being replaced too often detected!" << std::endl;
	return 0;
}

void ParticleSystem::respawnParticle(Particle& particle, const glm::vec3& particleCenterPosition)
{
	float X = (((rand() % 3001) - 1500) / 1000.0f); // width of spawning area
	float Y = 0;
	float Z = (((rand() % 1001) / 100.0f) - 30.0f); // along a line


	// "spawning position" of the particle
	particle.position = particleCenterPosition + glm::vec3(X, Y, Z); // +offset;

	// color of the particle

	const glm::vec3 rgbColor = USE_SRGB_COLORS ? glm::vec3(0.722, 0.318, 0.082) : glm::vec3(0.878, 0.624, 0.376);
	particle.color = glm::vec4(rgbColor, 0.0f);

	// lifetime of the particle (it gets removed/replaced after this)
	particle.life = this->_particleLifetime;

	// particle velocity (constant here)
	float velocityX = 3.0f * (((rand() % 2001) - 1000) / 1000.0f);
	float velocityY = -4.0f * ((rand() % 1001) / 1000.0f);
	float velocityZ = -(8.0f + 8.0f * ((rand() % 1001) / 1000.0f));
	particle.velocity = glm::vec3(velocityX, velocityY, velocityZ);

	particle.flag = false;

	particle.rotationRadians = glm::radians((((rand() % 72001) - 36000) / 100.0f)); // a random particle rotation. range: [-360, 360] degrees
}

void ParticleSystem::sortParticles(const glm::vec3& cameraPos)
{
	// very slow...
	std::sort(this->_particles.begin(), this->_particles.end(), [&](const Particle& one, const Particle& other)
	{
		const auto h1 = one.position - cameraPos;
		const auto distance2First = glm::dot(h1, h1);

		const auto h2 = other.position - cameraPos;
		const auto distance2Second = glm::dot(h2, h2);

		return distance2First > distance2Second;
	});
}

void ParticleSystem::drawParticleOnAlive(const Particle& p, Shader& particleShader)
{
	particleShader.setVec3("particleCenterWorld", p.position);
	particleShader.setVec4("color", p.color);
	particleShader.setMat2("rotate", WorldMathUtils::getRotationMatrix2D(p.rotationRadians));
	this->drawQuadWithTexture();
}

void ParticleSystem::updateAliveParticle(Particle& p, const float deltaTime)
{
	p.position -= p.velocity * deltaTime;

	if (p.color.a >= 1.0f && p.flag == false) p.flag = true;

	if (p.flag)
	{
		// once max visibility is reached, slowly decreases visibility again
		// so the most visibility is in the "middle"
		// I liked this effect as shown by this person: https://www.youtube.com/watch?v=nA-QGN0G5Pc
		p.color.a -= deltaTime * 0.1f;
	}
	else
	{
		// slowly gets more visible over time
		p.color.a += deltaTime * 0.2f;
	}

	p.rotationRadians += 0.005f;
}

void ParticleSystem::setupShaderForDraw(Shader& particleShader, const glm::mat4& view)
{
	// https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
	const glm::vec3 cameraRightWorldSpace = glm::vec3(view[0][0], view[1][0], view[2][0]);
	const glm::vec3 cameraUpWorldSpace = glm::vec3(view[0][1], view[1][1], view[2][1]);
	particleShader.setVec3("cameraRightWorldSpace", cameraRightWorldSpace);
	particleShader.setVec3("cameraUpWorldSpace", cameraUpWorldSpace);
	particleShader.setVec2("billboardSize", this->_particleSize);
	particleShader.setInt("sprite", 0); // texture
}

void ParticleSystem::drawQuadWithTexture()
{
	this->_quad.draw(this->_textureId);
}

WorldTimeManager* ParticleSystem::getTimeManager() const
{
	return this->_time;
}

unsigned int ParticleSystem::getNumParticles() const
{
	return this->_nrParticles;
}

float ParticleSystem::getParticleLifetime() const
{
	return this->_particleLifetime;
}

glm::vec3 ParticleSystem::getCurrentCenterPosition() const
{
	return this->_centerPosition;
}

glm::vec2 ParticleSystem::getParticleSize() const
{
	return this->_particleSize;
}
