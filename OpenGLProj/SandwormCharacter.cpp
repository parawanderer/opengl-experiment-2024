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
_time(time),
_terrain(terrain),
_sound(sound),
_model(sandwormGameObject),
_animator(animations),
_dustParticle1(dustParticle1),
_dustParticle2(dustParticle2),
_pitch(0.0f /* always 0 for now */),
_yaw(-180.0f),
_currentPos(terrain->getWorldHeightVecFor(initialX, initialZ)),
_currentFront(WorldMathUtils::computeDirection(this->_pitch, this->_yaw))
{
	this->playAnimationWithTransition(TPOSE_ANIM);
	this->_yPosOffset = UNDERGROUND_OFFSET;
	this->updateModelTransform();
}

SandWormCharacter::~SandWormCharacter()
{
	this->stopAndClearCurrentSounds();
}

void SandWormCharacter::onNewFrame()
{
	const float currentTime = this->_time->getCurrentTime();
	this->updateAnimationInterpolation();

	this->updateModelTransform();
	this->interpolateSound(currentTime);


	if (this->_nextBehaviourStage && this->_nextBehaviourStageAt > -1.0f && currentTime >= this->_nextBehaviourStageAt)
	{
		this->playAnimationWithTransition(this->_nextBehaviourStage->animationName);

		this->_nextBehaviourStageAt = -1.0f;
		this->_nextBehaviourStage = nullptr;
	}
	else if (currentTime < this->_nextBehaviourChoiceAt)
	{
		switch(this->_movementState)
		{
		case MOVEMENT_STATE::STATIC: // nothing
			return;
		case MOVEMENT_STATE::MOVING: // continue movement!
			this->interpolateMoveState(currentTime);
			break;
		}
	} else
	{
		this->_movementState = MOVEMENT_STATE::STATIC;
	}

	if (this->_showDust) this->updateParticlePlacement();

	if (this->_backgroundNoise) this->_backgroundNoise->setPosition(SoundManager::convert(this->getHeadPosition()));
	if (this->_inFrontNoise) this->_inFrontNoise->setPosition(SoundManager::convert(this->getHeadPosition()));
}

void SandWormCharacter::draw(Shader& shader)
{
	const std::vector<glm::mat4> transforms = this->_animator.getFinalBoneMatrices();
	this->setupEntityShaderForAnim(shader, transforms);

	this->_model->draw(shader);
	this->clearEntityShaderForAnim(shader);
}

void SandWormCharacter::startQuakingEarth()
{
	if (!this->_backgroundNoise)
	{
		this->_backgroundNoise = this->_sound->playTracked3D(BACKGROUND_RUMBLE_TRACK, true, this->getHeadPosition());
		this->_backgroundNoise->setMinDistance(BACKGROUND_RUMBLE_MIN_DISTANCE);
		this->_backgroundNoise->setVolume(0.0f);
		this->_earthquakeInterpolationStart = this->_time->getCurrentTime();

		this->playAnimationWithTransition(MOVING_BELOWGROUND_ANIM);
	}
}

void SandWormCharacter::appearAndMoveTowards(const glm::vec3& position)
{
	// TODO: this is duplicated from NomadCharacer -> move to shared place
	const float currentTime = this->_time->getCurrentTime();
	this->rotateTowards(position);

	this->_movementStartPos = this->_currentPos;
	this->_movementTarget = this->_terrain->getWorldHeightVecFor(position.x, position.z);
	this->_movementStartTime = currentTime;
	const float runDistance = glm::distance(this->_currentPos, position);
	this->_nextBehaviourChoiceAt = currentTime + (runDistance / MOVEMENT_SPEED_M_PER_SEC) + ANIMATION_TRANSITION_TIME;

	this->_movementState = MOVEMENT_STATE::MOVING;

	this->playAnimationWithTransition(CHASING_APPEARS_ABOVEGROUND_ANIM);
	this->startCreatingPrimaryDestructionNoise();
	this->queueBehaviourStage(CHASE_NORMAL, 1.2f);

	this->showDust(true);
}

void SandWormCharacter::stopAndClearCurrentSounds()
{
	if (this->_backgroundNoise != nullptr)
	{
		this->_backgroundNoise->stop();
		this->_backgroundNoise->drop();
		this->_backgroundNoise = nullptr;
	}

	if (this->_inFrontNoise != nullptr)
	{
		this->_inFrontNoise->stop();
		this->_inFrontNoise->drop();
		this->_inFrontNoise = nullptr;
	}
}

glm::vec3 SandWormCharacter::getHeadPosition() const
{
	return this->_currentPos + (this->_currentFront * HEAD_OFFSET_LENGTH_FROM_MIDDLE);
}

void SandWormCharacter::interpolateSound(const float currentTime)
{
	if (this->_backgroundNoise && currentTime <= this->_earthquakeInterpolationStart + SOUND_INTERPOLATION_DURATION_BG)
	{
		const float volume = 1.0f - (((this->_earthquakeInterpolationStart + SOUND_INTERPOLATION_DURATION_BG) - currentTime) / SOUND_INTERPOLATION_DURATION_BG);
		this->_backgroundNoise->setVolume(volume);
	}

	if (this->_inFrontNoise && currentTime <= this->_inFrontNoiseInterpolationStart + SOUND_INTERPOLATION_DURATION_FG)
	{
		const float volume = 1.0f - (((this->_inFrontNoiseInterpolationStart + SOUND_INTERPOLATION_DURATION_FG) - currentTime) / SOUND_INTERPOLATION_DURATION_FG);
		this->_inFrontNoise->setVolume(volume);
	}
}

void SandWormCharacter::updateModelTransform()
{
	glm::mat4 model = glm::mat4(1.0f);

	const float yOffset = this->_yPosOffset + this->getYDifferenceHeadAndBody(); // we want to offset relative to the head, not the middle of the body. So we need to convert this

	model = glm::translate(model, this->_currentPos + glm::vec3(0.0f, yOffset, 0.0f)); // place worm at current position
	model = glm::rotate(model, glm::radians(this->_yaw) + glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	model = glm::scale(model, glm::vec3(2.0f)); // rotate it
	this->_model->setModelTransform(model);
}

void SandWormCharacter::rotateTowards(const glm::vec3& position)
{ // TODO: this IS **very similar** to NomadCharacer -> move to shared place

	glm::vec3 newDirection = position - this->_currentPos;
	newDirection.y = 0.0f;
	newDirection = glm::normalize(newDirection);

	this->_currentFront = newDirection;

	this->_pitch = 0.0f;
	this->_yaw = glm::degrees(acos(glm::dot(this->_currentFront, glm::vec3(1.0, 0.0, 0.0))));
	if (glm::dot(this->_currentFront, glm::vec3(0.0, 0.0, 1.0)) > 0.0f)
		this->_yaw = 360.0f - this->_yaw;
	this->updateModelTransform();
}

const glm::vec3& SandWormCharacter::getCurrentPosition() const
{
	return this->_currentPos;
}

void SandWormCharacter::interpolateMoveState(const float currentTime)
{ // TODO: this is similar to NomadCharacer -> move to shared place
	float t = (currentTime - this->_movementStartTime) / (this->_nextBehaviourChoiceAt - this->_movementStartTime); // t = range [0, 1]
	// linear interpolation
	const glm::vec3 pos = (1 - t) * this->_movementStartPos + t * this->_movementTarget; // at t = 0, completely @ start, otherwise completely at end
	this->_currentPos = this->_terrain->getWorldHeightVecFor(pos.x, pos.z);
	this->_yPosOffset = std::min(_yPosOffset + GO_UP_PER_FRAME, MAX_OFFSET_Y);
	this->updateModelTransform();
}

void SandWormCharacter::playAnimationWithTransition(const std::string& animationName)
{ // TODO: this is duplicated from NomadCharacer -> move to shared place
	if (!this->_animation1.empty())
	{
		this->_animationInterpolationStart = this->_time->getCurrentTime();
		this->_animator.startPlaying2ndAnimation(
			this->_animator.getAnimationByName(animationName)
		);
		this->_animation2 = animationName;
		//std::cout << "Start interpolating " << this->_animation1 << " -> " << this->_animation2 << std::endl;
	}
	else
	{
		this->playAnimation(animationName);
	}
}

void SandWormCharacter::playAnimation(const std::string& animationName)
{ // TODO: this is duplicated from NomadCharacer -> move to shared place
	this->_animator.clearAnimation2();
	this->_animator.playAnimation(animationName);
	this->_animation1 = animationName;
}

void SandWormCharacter::updateAnimationInterpolation()
{ // TODO: this is duplicated from NomadCharacer -> move to shared place
	if (!this->_animation2.empty() && (this->_time->getCurrentTime() <= this->_animationInterpolationStart + ANIMATION_TRANSITION_TIME))
	{
		// interpolate between two animations based on time (within the time span of ANIMATION_TRANSITION_TIME)
		float interpolationFactor = (this->_time->getCurrentTime() - this->_animationInterpolationStart) / ANIMATION_TRANSITION_TIME;
		interpolationFactor = InterpolationMathUtil::easeInOutCosine(interpolationFactor);
		//std::cout << "Interpolating " << this->_animation1 << " -> " << this->_animation2 << " @ " << interpolationFactor << std::endl;
		this->_animator.updateAnimation2(this->_time->getDeltaTime(), interpolationFactor);
	}
	else
	{
		if (!this->_animation2.empty())
		{
			this->_animation1 = this->_animation2;
			this->_animation2 = ""; // reset the second. Now the first will be played *strictly*
			this->_animator.flipAnimation1And2();
			this->_animator.clearAnimation2();
		}
		this->_animator.updateAnimation(this->_time->getDeltaTime());
	}
}

void SandWormCharacter::queueBehaviourStage(const BehaviourStage& behaviourStage, const float executeAfterSeconds)
{
	this->_nextBehaviourStage = &behaviourStage;
	this->_nextBehaviourStageAt = this->_time->getCurrentTime() + executeAfterSeconds;
}

void SandWormCharacter::updateParticlePlacement()
{
	const glm::vec3 headPos = this->getHeadPosition();
	// we switch to 2d now because we can ignore "y" (which will be taken from the terrain)
	//const glm::vec3 head2DPos = glm::vec3(headPos.x, 0.0f, headPos.z);
	const glm::vec3 headDirXZPerpendicularUnitVector = glm::vec3(-this->_currentFront.z, 0.0f /* <- notably this is ignored, we take this perpendicular vector in 2D */, this->_currentFront.x);
	// ^ currently I won't normalize this as the worm can't really look up or down anyway, so this->_currentFront.y is necessarily always 0...

	// pos 1
	glm::vec3 posLeft = headPos + (this->_currentFront * PARTICLE_OFFSET_FRONT) + (HEAD_RADIUS * headDirXZPerpendicularUnitVector);
	posLeft = this->_terrain->getWorldHeightVecFor(posLeft.x, posLeft.z);

	// pos 2
	glm::vec3 posRight = headPos + (this->_currentFront * PARTICLE_OFFSET_FRONT) - (HEAD_RADIUS * headDirXZPerpendicularUnitVector);
	posRight = this->_terrain->getWorldHeightVecFor(posRight.x, posRight.z);

	const glm::vec3 spawnAlongVectorFromCenter = -this->_currentFront * PARTICLE_SPAWN_ALONG_LINE_LENGTH;

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
	if (!this->_inFrontNoise)
	{
		this->_inFrontNoise = this->_sound->playTracked3D(IN_FRONT_EARTH_BREAKING_TRACK, true, this->getHeadPosition());
		this->_inFrontNoise->setMinDistance(IN_FRONT_EARTH_RIPPING_MIN_DISTANCE);
		this->_inFrontNoise->setVolume(0.0f);
		this->_inFrontNoiseInterpolationStart = this->_time->getCurrentTime();
	}
}
