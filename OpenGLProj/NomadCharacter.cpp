#include "NomadCharacter.h"

#include <iostream>
#include <glm/ext/matrix_transform.hpp>

#define WALKING_SPEED_M_PER_SEC 1.42f
#define SMALL_Y_OFFSET  glm::vec3(0.0f, 0.1f, 0.0f)

// we can seed the random generator or not. Doesn't really matter for now.

NomadCharacter::NomadCharacter(WorldTimeManager* time, Terrain* terrain, RenderableGameObject* nomadGameObject, float initialX, float initialZ) :
	_time(time),
	_terrain(terrain),
	_nomadModel(nomadGameObject),
	_currentPos(terrain->getWorldHeightVecFor(initialX, initialZ) + SMALL_Y_OFFSET),
	_currentFront(glm::vec3(0.0, 0.0, -1.0))
{}

void NomadCharacter::onNewFrame()
{
	// decide if want to stand around or walk for the next x 
	const float currentTime = this->_time->getCurrentTime();
	if (currentTime <= this->_nextBehaviourChoiceAt)
	{
		if (this->_movementState == MOVEMENT_STATE::IDLE) {
			return;
		}
		else if (this->_movementState == MOVEMENT_STATE::WALKING)
		{
			float t = (currentTime - this->_movementStartTime) / (this->_nextBehaviourChoiceAt - this->_movementStartTime); // t = range [0, 1]
			// linear interpolation
			const glm::vec3 pos = (1 - t) * this->_movementStartPos + t * this->_movementTarget; // at t = 0, completely @ start, otherwise completely at end
			this->_currentPos = this->_terrain->getWorldHeightVecFor(pos.x, pos.z) + SMALL_Y_OFFSET;
			//std::cout << "x: " << this->_currentPos.x << ", y: " << this->_currentPos.y << ", z: " << this->_currentPos.z << "\n";
		}
	}
	else
	{
		bool wantsToWalkSomewhere = rand() % 2;
		//std::cout << "wants to walk?: " << wantsToWalkSomewhere << "\n";
		if (!wantsToWalkSomewhere)
		{
			this->_nextBehaviourChoiceAt = currentTime + (rand() % 10 + 1); // <- seconds
			this->_movementState = MOVEMENT_STATE::IDLE;
			return;
		}

		// **how far** the character wants to walk (in world units)
		const float desiredWalkDistance = rand() % 10 + 1;
		const float angle = rand() % 360 + 1 - 180;

		this->_yaw += angle;
		if (this->_yaw < 0) this->_yaw += 360.0f;
		if (this->_yaw > 360.0f) this->_yaw -= 360.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));
		direction.y = sin(glm::radians(this->_pitch));
		direction.z = sin(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));
		this->_currentFront = glm::normalize(direction);

		this->_movementStartPos = this->_currentPos;
		this->_movementTarget = this->_currentPos + (this->_currentFront) * desiredWalkDistance; // interpolate between them? (ignore "y" dimension -> get it from the height map)
		this->_movementStartTime = currentTime;
		this->_nextBehaviourChoiceAt = currentTime + (desiredWalkDistance / WALKING_SPEED_M_PER_SEC); // (seconds) (character will walk desiredWalkDistance in this amount of seconds if he walks at WALKING_SPEED_M_PER_SEC)
		this->_movementState = MOVEMENT_STATE::WALKING;
	}

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->_currentPos); // place character at current position
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // orient character along his movement direction
	model = glm::rotate(model, -glm::radians(this->_yaw), glm::vec3(0.0f, 1.0f, 0.0f));        // same ^
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate the model into upward position
	this->_nomadModel->setModelTransform(model);
}

void NomadCharacter::draw(Shader& shader)
{
	this->_nomadModel->draw(shader);
}
