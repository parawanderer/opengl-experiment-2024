#include "Thumper.h"

// I animated these little things myself on the model
const std::string DISABLED_ANIM = "static";
const std::string ACTIVE_ANIM = "thump";
const std::string ROTATING_TOP_ANIM = "activate";

const float THUMPER_MODEL_SCALE = 1.0f;

Thumper::Thumper(WorldTimeManager* time, SphericalBoxedGameObject* thumper, AnimationSet* animations):
_time(time),
_model(thumper),
_animator(animations)
{
	this->_animator.playAnimation(DISABLED_ANIM);
}

void Thumper::onNewFrame()
{
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
			this->_animator.playAnimation(DISABLED_ANIM);
			break;
		case STATE::ACTIVATED:
			this->_animator.playAnimation(ACTIVE_ANIM);
			break;
		}
	}
}

Thumper::STATE Thumper::getState() const
{
	return this->_state;
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

void Thumper::setModelTransform(const glm::mat4& model)
{
	glm::mat4 modelFull = model * glm::scale(glm::mat4(1.0), glm::vec3(THUMPER_MODEL_SCALE));
	this->_model->setModelTransform(modelFull);
}
