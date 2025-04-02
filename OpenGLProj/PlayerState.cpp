#include "PlayerState.h"

#include <iostream>

const std::string SAND_WALKING_TRACK = "006142914-footsteps-walking-sand-01-loop.mp3";
const std::string SAND_RUNNING_TRACK = "098878381-sand-running-footsteps-loop-01.mp3";
const std::string SAND_JUMP_END_TACK = "singlestep-sand.mp3";
const std::string SAND_JUMP_START_TRACK = "jumpstep-sand.mp3";

PlayerState::PlayerState(SoundManager* sound, const PlayerCamera* playerCamera):
_sound(sound),
_playerCamera(playerCamera),
_carriedItem(nullptr)
{}

bool PlayerState::hasCarriedItem()
{
	return this->_hasItem;
}

void PlayerState::setCarriedItem(const CarriedGameObject& item)
{
	this->_hasItem = true;
	this->_carriedItem = item;
}

CarriedGameObject& PlayerState::getCarriedItem()
{
	return this->_carriedItem;
}

CarriedGameObject PlayerState::removeCarriedItem()
{
	this->_hasItem = false;
	const auto item = this->_carriedItem;
	this->_carriedItem = CarriedGameObject(nullptr);
	return item;
}

void PlayerState::onNewPos(const glm::vec3& newPos)
{
	const glm::vec3 underFeetPos = this->getUnderFeetPos(newPos);
	this->_currentWalkSound.setPosition(underFeetPos);
	this->_currentJumpSound.setPosition(underFeetPos);

	if (!this->_currentWalkSound.hasAudio() && this->_currentJumpSound.hasAudio() && !this->_isInAirDueToJump)
	{
		// we are still moving, probably stopped jumping but still moving so want to start playing the appropriate sound effect again
		if (this->_movementState == 2)
		{
			this->onStartRunning();
		}
		else if (this->_movementState == 1)
		{
			this->onStartWalking();
		}
	}
}

void PlayerState::onStartWalking()
{
	this->_movementState = 1;
	this->_currentWalkSound = this->_sound->playTracked3D(SAND_WALKING_TRACK, true, this->getUnderFeetPos(this->_playerCamera->getPos()));
}

void PlayerState::onStopMoving()
{
	this->_movementState = 0;
	this->_currentWalkSound.stopAndRelease();
}

void PlayerState::onStartJumping()
{
	this->_currentWalkSound.stopAndRelease();
	this->_currentJumpSound = this->_sound->playTracked3D(SAND_JUMP_START_TRACK, false, this->getUnderFeetPos(this->_playerCamera->getPos()));
	this->_isInAirDueToJump = true;
}

void PlayerState::onStopJumping()
{
	this->_currentJumpSound = this->_sound->playTracked3D(SAND_JUMP_END_TACK, false, this->getUnderFeetPos(this->_playerCamera->getPos()));
	this->_isInAirDueToJump = false;
}

void PlayerState::onStartRunning()
{
	this->_movementState = 2;
	this->_currentWalkSound = this->_sound->playTracked3D(SAND_RUNNING_TRACK, true, this->getUnderFeetPos(this->_playerCamera->getPos()));
}

glm::vec3 PlayerState::getUnderFeetPos(const glm::vec3& playerPos) const
{
	return playerPos - glm::vec3(0.0, this->_playerCamera->getPlayerHeight(), 0.0);
}
