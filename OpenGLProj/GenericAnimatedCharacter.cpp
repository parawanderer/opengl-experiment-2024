#include "GenericAnimatedCharacter.h"

#include "InterpolationMathUtil.h"
#include "WorldMathUtils.h"

constexpr auto FIXED_PITCH = 0.0f;  /* always 0 for now */


GenericAnimatedCharacter::GenericAnimatedCharacter(
	const WorldTimeManager* time, 
	RenderableGameObject* model, 
	AnimationSet* animations,
	const float animationTransitionTime,
	const glm::vec3& initialPosition,
	const float initialYaw,
	const float yawModelOffset,
	const glm::vec3& rotateYawOver
):
_time(time),
_model(model),
_animator(animations),
_currentPosition(initialPosition),
_currentFront(WorldMathUtils::computeDirection(FIXED_PITCH, initialYaw)),
_yaw(initialYaw),
_yawModelOffset(yawModelOffset),
_rotateYawOver(rotateYawOver),
_animationTransitionTime(animationTransitionTime)
{}

void GenericAnimatedCharacter::draw(Shader& shader)
{
	const std::vector<glm::mat4> transforms = this->_animator.getFinalBoneMatrices();
	this->setupEntityShaderForAnim(shader, transforms);

	this->_model->draw(shader);
	this->clearEntityShaderForAnim(shader);
}

glm::vec3 GenericAnimatedCharacter::getCurrentPosition() const
{
	return this->_currentPosition;
}

const glm::vec3& GenericAnimatedCharacter::getCurrentFront() const
{
	return this->_currentFront;
}

float GenericAnimatedCharacter::getYaw() const
{
	return this->_yaw;
}

float GenericAnimatedCharacter::getPitch() const
{
	return FIXED_PITCH;
}

void GenericAnimatedCharacter::rotateTowards(const glm::vec3& position)
{
	// TODO: currently characters can't really look up or down, but it might be nice to add this at some point
	glm::vec3 newDirection = position - this->_currentPosition;
	newDirection.y = 0.0f;
	newDirection = glm::normalize(newDirection);

	this->_currentFront = newDirection;

	this->_yaw = glm::degrees(acos(glm::dot(this->_currentFront, this->_rotateYawOver))) + this->_yawModelOffset;
	if (glm::dot(this->_currentFront, glm::vec3(0.0, 0.0, 1.0)) > 0.0f)
		this->_yaw = 360.0f - this->_yaw;

	this->updateModelTransform();
}

Animator& GenericAnimatedCharacter::getAnimator()
{
	return this->_animator;
}

RenderableGameObject* GenericAnimatedCharacter::getRenderableGameModel() const
{
	return this->_model;
}

const WorldTimeManager* GenericAnimatedCharacter::getTime() const
{
	return this->_time;
}

void GenericAnimatedCharacter::setCurrentPosition(const glm::vec3& newPosition)
{
	this->_currentPosition = newPosition;
}

void GenericAnimatedCharacter::setCurrentFront(const glm::vec3& newFront)
{
	this->_currentFront = newFront;
}

void GenericAnimatedCharacter::setYaw(const float newYaw)
{
	this->_yaw = newYaw;
}

void GenericAnimatedCharacter::playAnimation(const std::string& animationName)
{
	this->_animator.clearAnimation2();
	this->_animator.playAnimation(animationName);
	this->_animation1 = animationName;
}

void GenericAnimatedCharacter::playAnimationWithTransition(const std::string& animationName)
{
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

void GenericAnimatedCharacter::updateAnimationInterpolationForFrame()
{
	if (!this->_animation2.empty() && (this->_time->getCurrentTime() <= this->_animationInterpolationStart + this->_animationTransitionTime))
	{
		// interpolate between two animations based on time (within the time span of ANIMATION_TRANSITION_TIME)
		float interpolationFactor = (this->_time->getCurrentTime() - this->_animationInterpolationStart) / this->_animationTransitionTime;
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
