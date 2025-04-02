#include "SandWormCharacter.h"

#include <iostream>

#include "InterpolationMathUtil.h"
#include "WorldMathUtils.h"


constexpr auto UNDERGROUND_OFFSET = -35.0f;
constexpr auto MOVEMENT_SPEED_M_PER_SEC = 30.0f;
constexpr auto ANIMATION_TRANSITION_TIME = 0.2f;
constexpr auto HEAD_RADIUS = 24.0f; // in meters
constexpr auto PARTICLE_OFFSET_FRONT = 70.0f; // in meters
constexpr auto PARTICLE_SPAWN_ALONG_LINE_LENGTH = 8.0f; // in meters
constexpr auto MAX_OFFSET_Y = 0.0f; // in meters
constexpr auto GO_UP_PER_FRAME = 0.1f;
constexpr auto INITIAL_YAW = -180.0f;

// animations that I made myself (as I made this model myself, initially following along with a tutorial for the model itself)
const std::string TPOSE_ANIM = "tpose";
const std::string CHASE_EAT_ANIM = "chase_eat";
const std::string CHASING_ABOVEGROUND_ANIM = "chasing";
const std::string CHASING_APPEARS_ABOVEGROUND_ANIM = "moveappears";
const std::string MOVING_BELOWGROUND_ANIM = "moveunder";


const std::string BACKGROUND_RUMBLE_TRACK = "distant_earth_rumble.mp3";
const float BACKGROUND_RUMBLE_MIN_DISTANCE = 250.0f;
const std::string IN_FRONT_EARTH_BREAKING_TRACK = "ripping_earth.mp3";
const float IN_FRONT_EARTH_RIPPING_MIN_DISTANCE = 20.0f;


const float HEAD_OFFSET_LENGTH_FROM_MIDDLE = 150.0f; // the head is 150m removed from the middle of the character

const float SOUND_INTERPOLATION_DURATION_BG = 30.0f; // in seconds
const float SOUND_INTERPOLATION_DURATION_FG = 10.0f; // in seconds


const BehaviourStage CHASE_NORMAL = {
	.animationName = CHASING_ABOVEGROUND_ANIM,
	.muteNoise = false
};

const BehaviourStage DISAPPEAR_BELOW_GROUND = {
	.animationName = MOVING_BELOWGROUND_ANIM,
	.muteNoise = true
};


SandWormCharacter::SandWormCharacter(
	const WorldTimeManager* time,
	const Terrain* terrain,
	SoundManager* sound,
	RenderableGameObject* sandwormGameObject,
	AnimationSet* animations,
	ParticleSystem* dustParticle1,
	ParticleSystem* dustParticle2,
	float initialX, 
	float initialZ)
:
GenericAnimatedCharacter(
	time,
	sandwormGameObject,
	animations,
	ANIMATION_TRANSITION_TIME,
	terrain->getWorldHeightVecFor(initialX, initialZ),
	INITIAL_YAW,
	0.0f,
	glm::vec3(1.0, 0.0, 0.0)
),
_terrain(terrain),
_sound(sound),
_model(sandwormGameObject),
_dustParticle1(dustParticle1),
_dustParticle2(dustParticle2)
{
	this->GenericAnimatedCharacter::playAnimationWithTransition(TPOSE_ANIM);
	this->_yPosOffset = UNDERGROUND_OFFSET;
	this->SandWormCharacter::updateModelTransform();
}

void SandWormCharacter::onNewFrame()
{
	const float currentTime = this->getTime()->getCurrentTime();
	this->updateAnimationInterpolationForFrame();

	this->updateModelTransform();
	this->interpolateSound(currentTime);

	if (this->hasEnqueuedBehaviourStagePending(currentTime))
	{
		this->playAnimationWithTransition(this->_nextBehaviourStage->animationName);

		this->_nextBehaviourStageAt = -1.0f;
		this->_nextBehaviourStage = nullptr;
	}
	else if (currentTime < this->_movementEndTime)
	{
		switch(this->_movementState)
		{
		case MOVEMENT_STATE::STATIC: // nothing
			return;
		case MOVEMENT_STATE::MOVING: // continue movement!
			this->interpolateMoveState(currentTime);
			break;
		}
	}
	else
	{
		this->_movementState = MOVEMENT_STATE::STATIC;
	}

	if (this->_showDust) this->updateParticlePlacement();

	const glm::vec3 headPos = this->getHeadPosition();
	this->_backgroundNoise.setPosition(headPos);
	this->_foregroundNoise.setPosition(headPos);
}

void SandWormCharacter::startQuakingEarth()
{
	if (!this->_backgroundNoise.hasAudio())
	{
		this->_backgroundNoise = this->_sound->playTracked3D(BACKGROUND_RUMBLE_TRACK, true, this->getHeadPosition());
		this->_backgroundNoise.setMinimumDistance(BACKGROUND_RUMBLE_MIN_DISTANCE);
		this->_backgroundNoise.setVolume(0.0f);
		this->_bgNoiseInterpolationStart = this->getTime()->getCurrentTime();

		this->playAnimationWithTransition(MOVING_BELOWGROUND_ANIM);
	}
}

void SandWormCharacter::appearAndMoveTowards(const glm::vec3& position)
{
	const float currentTime = this->getTime()->getCurrentTime();
	this->rotateTowards(position);

	this->_movementStartPos = this->getCurrentPosition();
	this->_movementTarget = this->_terrain->getWorldHeightVecFor(position.x, position.z);
	this->_movementStartTime = currentTime;
	const float runDistance = glm::distance(this->getCurrentPosition(), position);
	this->_movementEndTime = currentTime + (runDistance / MOVEMENT_SPEED_M_PER_SEC) + ANIMATION_TRANSITION_TIME;

	this->_movementState = MOVEMENT_STATE::MOVING;

	this->playAnimationWithTransition(CHASING_APPEARS_ABOVEGROUND_ANIM);
	this->startCreatingPrimaryDestructionNoise();
	this->enqueueBehaviourStage(CHASE_NORMAL, 1.2f);
	this->showDust(true);
}

glm::vec3 SandWormCharacter::getHeadPosition() const
{
	return this->getCurrentPosition() + (this->getCurrentFront() * HEAD_OFFSET_LENGTH_FROM_MIDDLE);
}

void SandWormCharacter::interpolateSound(const float currentTime)
{
	SandWormCharacter::interpolateSpecificSound(currentTime, this->_backgroundNoise, this->_bgNoiseInterpolationStart, SOUND_INTERPOLATION_DURATION_BG);
	SandWormCharacter::interpolateSpecificSound(currentTime, this->_foregroundNoise, this->_fgInterpolationStart, SOUND_INTERPOLATION_DURATION_FG);
}

void SandWormCharacter::interpolateSpecificSound(
	const float currentTime, 
	AudioPlayer& soundToInterpolate, 
	const float interpolationStartTime,
	const float interpolationDuration)
{
	if (soundToInterpolate.hasAudio() && currentTime <= interpolationStartTime + interpolationDuration)
	{
		const float volume = 1.0f - (((interpolationStartTime + interpolationDuration) - currentTime) / interpolationDuration);
		soundToInterpolate.setVolume(volume);
	}
}

void SandWormCharacter::updateModelTransform()
{
	glm::mat4 model = glm::mat4(1.0f);

	const float yOffset = this->_yPosOffset + this->getYDifferenceHeadAndBody(); // we want to offset relative to the head, not the middle of the body. So we need to convert this

	model = glm::translate(model, this->getCurrentPosition() + glm::vec3(0.0f, yOffset, 0.0f)); // place worm at current position
	model = glm::rotate(model, glm::radians(this->getYaw()) + glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	model = glm::scale(model, glm::vec3(2.0f)); // rotate it
	this->_model->setModelTransform(model);
}

void SandWormCharacter::interpolateMoveState(const float currentTime)
{
	float t = (currentTime - this->_movementStartTime) / (this->_movementEndTime - this->_movementStartTime); // t = range [0, 1]
	// linear interpolation
	const glm::vec3 pos = (1 - t) * this->_movementStartPos + t * this->_movementTarget; // at t = 0, completely @ start, otherwise completely at end
	this->setCurrentPosition(this->_terrain->getWorldHeightVecFor(pos.x, pos.z));
	this->_yPosOffset = std::min(_yPosOffset + GO_UP_PER_FRAME, MAX_OFFSET_Y);
	this->updateModelTransform();
}

void SandWormCharacter::enqueueBehaviourStage(const BehaviourStage& behaviourStage, const float executeAfterSeconds)
{
	this->_nextBehaviourStage = &behaviourStage;
	this->_nextBehaviourStageAt = this->getTime()->getCurrentTime() + executeAfterSeconds;
}

bool SandWormCharacter::hasEnqueuedBehaviourStagePending(const float currentTime) const
{
	return this->_nextBehaviourStage && this->_nextBehaviourStageAt > -1.0f && currentTime >= this->_nextBehaviourStageAt;
}

void SandWormCharacter::updateParticlePlacement()
{
	const glm::vec3 headPos = this->getHeadPosition();
	const glm::vec3& currentFront = this->getCurrentFront();
	// we switch to 2d now because we can ignore "y" (which will be taken from the terrain)
	//const glm::vec3 head2DPos = glm::vec3(headPos.x, 0.0f, headPos.z);
	const glm::vec3 headDirXZPerpendicularUnitVector = glm::vec3(
		-currentFront.z, 
		0.0f /* <- notably this is ignored, we take this perpendicular vector in 2D */, 
		currentFront.x
	);
	// ^ currently I won't normalize this as the worm can't really look up or down anyway, so currentFront.y is necessarily always 0...

	// pos 1
	glm::vec3 posLeft = headPos + (currentFront * PARTICLE_OFFSET_FRONT) + (HEAD_RADIUS * headDirXZPerpendicularUnitVector);
	posLeft = this->_terrain->getWorldHeightVecFor(posLeft.x, posLeft.z);

	// pos 2
	glm::vec3 posRight = headPos + (currentFront * PARTICLE_OFFSET_FRONT) - (HEAD_RADIUS * headDirXZPerpendicularUnitVector);
	posRight = this->_terrain->getWorldHeightVecFor(posRight.x, posRight.z);

	const glm::vec3 spawnAlongVectorFromCenter = -currentFront * PARTICLE_SPAWN_ALONG_LINE_LENGTH;

	// apply
	this->_dustParticle1->setCenterPosition(posLeft);
	this->_dustParticle1->setSpawnAlongVector(spawnAlongVectorFromCenter);

	this->_dustParticle2->setCenterPosition(posRight);
	this->_dustParticle2->setSpawnAlongVector(spawnAlongVectorFromCenter);
}

void SandWormCharacter::showDust(bool doShow)
{
	this->_showDust = doShow;
	this->_dustParticle1->setNewParticlesEnabled(doShow);
	this->_dustParticle2->setNewParticlesEnabled(doShow);
}

float SandWormCharacter::getYDifferenceHeadAndBody() const
{
	const glm::vec3 headPos = this->getHeadPosition();
	const glm::vec3 middleOfBodyPos = this->getCurrentPosition();
	const float middleOfBodyY = this->_terrain->getWorldHeightVecFor(middleOfBodyPos.x, middleOfBodyPos.z).y;
	const float headY = this->_terrain->getWorldHeightVecFor(headPos.x, headPos.z).y;
	const float yDiff = headY - middleOfBodyY;
	return yDiff;
}

void SandWormCharacter::startCreatingPrimaryDestructionNoise()
{
	if (!this->_foregroundNoise.hasAudio())
	{
		this->_foregroundNoise = this->_sound->playTracked3D(IN_FRONT_EARTH_BREAKING_TRACK, true, this->getHeadPosition());
		this->_foregroundNoise.setMinimumDistance(IN_FRONT_EARTH_RIPPING_MIN_DISTANCE);
		this->_foregroundNoise.setVolume(0.0f);
		this->_fgInterpolationStart = this->getTime()->getCurrentTime();
	}
}
