#include "ParticleSystem.h"

#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include "Colors.h"
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
	const glm::vec3& particlesVelocity,
	float particleLifetime,
	int particleCount,
	const glm::vec2& particleSize)
:
_time(time),
_nrParticles(particleCount),
_particleLifetime(particleLifetime),
_centerPosition(particlesCenter),
_velocity(particlesVelocity),
_particleSize(particleSize)
{
	// particle texture
	this->_textureId = loadTextureFromFile(particleTexturePath.c_str(), PROJ_CURRENT_DIR, GL_TEXTURE0);
	
	for (unsigned int i = 0; i < this->_nrParticles; ++i)
	{
		this->_particles.push_back(Particle()); // defaults
	}
}

void ParticleSystem::onNewFrame()
{
	const float deltaTime = this->_time->getDeltaTime();
	
	const glm::vec3 offset(0.0, 0.1f, 0.0); // TODO: make this dynamic
	
	unsigned int nrNewParticles = 2;
	// add new particles
	for (unsigned int i = 0; i < nrNewParticles; ++i)
	{
		int unusedParticle = this->findUnusedParticle();
		this->respawnParticle(
			this->_particles[unusedParticle], 
			this->_centerPosition,
			this->_velocity,
			offset
		);
	}
	// update all particles
	for (unsigned int i = 0; i < this->_nrParticles; ++i)
	{
		Particle& p = this->_particles[i];
		p.life -= deltaTime; // reduce life

		if (p.life > 0.0f) // particle is alive, thus update it:
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

	// https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
	particleShader.use();
	//particleShader.setVec3("particleCenterWorld", this->_centerPosition);
	const glm::vec3 cameraRightWorldSpace = glm::vec3(view[0][0], view[1][0], view[2][0]);
	const glm::vec3 cameraUpWorldSpace = glm::vec3(view[0][1], view[1][1], view[2][1]);
	particleShader.setVec3("cameraRightWorldSpace", cameraRightWorldSpace);
	particleShader.setVec3("cameraUpWorldSpace", cameraUpWorldSpace);
	particleShader.setVec2("billboardSize", this->_particleSize);
	particleShader.setInt("sprite", 0); // texture
	
	for (const Particle& p: this->_particles)
	{
		if (p.life > 0.0f)
		{
			particleShader.setVec3("particleCenterWorld", p.position);
			particleShader.setVec4("color", p.color);
			particleShader.setMat2("rotate", WorldMathUtils::getRotationMatrix2D(p.rotationRadians));
			this->_quad.draw(this->_textureId);
		}
	}

	glDisable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
	//std::cout << "-----------" << std::endl;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::setCenterPosition(const glm::vec3& position)
{
	this->_centerPosition = position;
}

void ParticleSystem::setVelocity(const glm::vec3& velocity)
{
	this->_velocity = velocity;
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
	for (unsigned i = 0; i < this->_lastUsedParticle; ++i)
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

void ParticleSystem::respawnParticle(
	Particle& particle, 
	const glm::vec3& particleCenterPosition,
	const glm::vec3& particleVelocity, 
	const glm::vec3& offset)
{

	// "Source" position
	// float random = (((rand() % 2001) - 1000) / 1000.0f);
	// float X = random;
	// float maxZ = sqrt(1 - X*X);
	// float Z = (((rand() % 1001) - 1000) / 1000.0f) * maxZ;
	// bool pos = rand() % 2;
	// if (pos) Z = -Z;
	//float Y = 1.0f;

	float X = (((rand() % 3001) - 1500) / 1000.0f); // width of spawning area
	float Y = 0;
	float Z = (((rand() % 10001) / 100.0f) - 20.0f); // along a line

	//float rColor = 0.5f + ((rand() % 100) / 100.0f); // random brightness!

	// "spawning position" of the particle
	particle.position = particleCenterPosition + glm::vec3(X, Y, Z); // +offset;

	// color of the particle
	particle.color = glm::vec4(0.878, 0.624, 0.376, 0.0f);

	// lifetime of the particle (it gets removed/replaced after this)
	particle.life = this->_particleLifetime;

	// particle velocity (constant here)
	float velocityX = 2.0f * (((rand() % 2001) - 1000) / 1000.0f);
	float velocityY = -4.0f * ((rand() % 1001) / 1000.0f);
	float velocityZ = -(8.0f + 8.0f * ((rand() % 1001) / 1000.0f));
	particle.velocity = glm::vec3(velocityX, velocityY, velocityZ);

	particle.flag = false;

	particle.rotationRadians = glm::radians((((rand() % 72001) - 36000) / 100.0f)); // a random particle rotation. range: [-360, 360] degrees
}

void ParticleSystem::sortParticles(const glm::vec3& cameraPos)
{
	std::sort(this->_particles.begin(), this->_particles.end(), [&](const Particle& one, const Particle& other)
	{
		const auto h1 = one.position - cameraPos;
		const auto distance2First = glm::dot(h1, h1);

		const auto h2 = other.position - cameraPos;
		const auto distance2Second = glm::dot(h2, h2);

		return distance2First > distance2Second;
	});
}
