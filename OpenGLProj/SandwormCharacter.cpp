#include "SandWormCharacter.h"

#define UNDERGROUND_OFFSET glm::vec3(0.0f, -22.0f, 0.0f)


// animations that I made myself (as I made this model myself, initially following along with a tutorial for the model itself)
const std::string TPOSE_ANIM = "tpose";
const std::string CHASE_EAT_ANIM = "chase_eat";
const std::string CHASING_ABOVEGROUND_ANIM = "chasing";
const std::string CHASING_APPEARS_ABOVEGROUND_ANIM = "moveappears";
const std::string MOVING_BELOWGROUND_ANIM = "moveunder";

SandWormCharacter::SandWormCharacter(
	const WorldTimeManager* time,
	const Terrain* terrain,
	RenderableGameObject* 
	sandwormGameObject,
	AnimationSet* animations, 
	float initialX, 
	float initialZ)
:
	_time(time),
	_terrain(terrain),
	_model(sandwormGameObject),
	_animator(animations),
	_currentPos(terrain->getWorldHeightVecFor(initialX, initialZ) + UNDERGROUND_OFFSET),
	_currentFront(glm::vec3(0.0, 0.0, -1.0))
{
	this->_animator.playAnimation(CHASING_ABOVEGROUND_ANIM);
}

void SandWormCharacter::onNewFrame()
{
	this->_animator.updateAnimation(this->_time->getDeltaTime());

	// TODO: do movement logic!!!

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->_currentPos); // place worm at current position
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0)); // rotate it
	model = glm::scale(model, glm::vec3(2.0f)); // rotate it
	this->_model->setModelTransform(model);
}

void SandWormCharacter::draw(Shader& shader)
{
	const std::vector<glm::mat4> transforms = this->_animator.getFinalBoneMatrices();
	this->setupEntityShaderForAnim(shader, transforms);

	this->_model->draw(shader);
	this->clearEntityShaderForAnim(shader);
}
