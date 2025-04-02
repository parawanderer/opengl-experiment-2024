#include "Thumper.h"

#include "CameraUtils.h"


const std::string WINDUP_TRACK = "162806__timgormly__wind-up2.mp3";
const std::string THUMPING_TRACK = "thump.mp3";
const std::string THUMPER_INSERT_INTO_SAND_TRACK = "sand_insert_thumper.mp3";
const std::string THUMPER_PULL_OUT_OF_SAND_TRACK = "sand_pullout_thumper.mp3";

const float THUMPER_SOUND_MIN_DISTANCE = 1.0f;


// I animated these little things myself on the model
const std::string DISABLED_ANIM = "static";
const std::string ACTIVE_ANIM = "thump";
const std::string ROTATING_TOP_ANIM = "activate";


const float THUMPER_MODEL_SCALE = 0.7f;


Thumper::Thumper(const WorldTimeManager* time, SoundManager* sound, SphericalBoxedGameObject* thumper, AnimationSet* animations):
_time(time),
_sound(sound),
_model(thumper),
_animator(animations)
{
	this->_animator.playAnimation(DISABLED_ANIM);
}

void Thumper::onNewFrame()
{
	if (this->_state == STATE::ACTIVATED && this->_isActivating && this->_time->getCurrentTime() >= this->_activatingSwitchToActiveAt)
	{
		// switch from the "activating" animation to the "active" animation when the "activating" animation is done playing
		this->_isActivating = false;
		this->_animator.playAnimation(ACTIVE_ANIM);

		// this particular track and the animation diverge over time even though they seem pretty close initially.
		// floating point error accumulation? Or is it just a matter of the actual difference in lengths?
		// Or is it that one of these things runs faster than expected for some reason? It doesn't _really matter for a demo of my project
		// but I might want to fix this at some point
		this->_currentSound = this->_sound->playTracked3D(THUMPING_TRACK, true, this->_currentPos);
		this->_currentSound.setMinimumDistance(THUMPER_SOUND_MIN_DISTANCE);
	}

	this->_animator.updateAnimation(this->_time->getDeltaTime());
}

void Thumper::draw(Shader& shader)
{
	const std::vector<glm::mat4> transforms = this->_animator.getFinalBoneMatrices();
	this->setupEntityShaderForAnim(shader, transforms);

	this->_model->draw(shader);
	this->clearEntityShaderForAnim(shader);
}

void Thumper::setState(STATE newState)
{
	if (this->_state != newState)
	{
		this->_state = newState;

		switch(newState)
		{
		case STATE::DISABLED:
			this->handleDeactivation();
			break;
		case STATE::ACTIVATED:
			this->handleActivation();
			break;
		}
	}
}

Thumper::STATE Thumper::getState() const
{
	return this->_state;
}

void Thumper::setIsCarried(bool isCarried)
{
	if (this->_isCarried != isCarried)
	{
		if (isCarried)
		{
			this->_currentSound = this->_sound->playTracked3D(THUMPER_PULL_OUT_OF_SAND_TRACK, false, this->_currentPos);
			this->_currentPos = glm::vec3(0.0, 0.0, 0.0);
		}
		else
		{
			this->_currentSound = this->_sound->playTracked3D(THUMPER_INSERT_INTO_SAND_TRACK, false, this->_currentPos);
		}
		this->_isCarried = isCarried;
	}
}

SphericalBoxedGameObject* Thumper::getObjectModel() const
{
	return this->_model;
}

float Thumper::getRadiusSphericalBoundingBox()
{
	return this->_model->getBoundRadius();
}

glm::vec3 Thumper::getBoundMidPoint()
{
	return this->_model->getWorldMidPoint();
}

void Thumper::setPosition(const glm::vec3& newPosition)
{
	this->_currentPos = newPosition;
	this->updateModelTransform();
}

void Thumper::drawCarried(Shader& shader, const glm::mat4& view, const float t, bool isMoving, bool isSpeeding)
{
	glm::mat4 model = CameraUtils::getCarriedItemModelTransform(view, t, isMoving, isSpeeding);
	this->_model->setModelTransform(model);
	this->draw(shader);
}

void Thumper::updateModelTransform()
{
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, this->_currentPos);
	model = glm::scale(model, glm::vec3(THUMPER_MODEL_SCALE));
	this->_model->setModelTransform(model);
}

void Thumper::handleActivation()
{
	Animation* activatingAnimation = this->_animator.getAnimationByName(ROTATING_TOP_ANIM);

	this->_activatingSwitchToActiveAt = this->_time->getCurrentTime() + (activatingAnimation->getDuration() / activatingAnimation->getTicksPerSecond());
	this->_isActivating = true;
	this->_state = STATE::ACTIVATED;

	this->_animator.playAnimation(activatingAnimation);
	this->_currentSound = this->_sound->playTracked3D(WINDUP_TRACK, false, this->_currentPos);
}

void Thumper::handleDeactivation()
{
	this->_animator.playAnimation(DISABLED_ANIM);
	this->_currentSound.stopAndRelease();
}