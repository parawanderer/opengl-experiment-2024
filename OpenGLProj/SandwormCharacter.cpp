#include "SandWormCharacter.h"

#define UNDERGROUND_OFFSET glm::vec3(0.0f, -22.0f, 0.0f)


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

// temporary until I introduce actual behaviour for this guy. Then the offset will not be fixed like this anymore...
#define HEAD_OFFSET_VECTOR	glm::vec3(0.0, 0.0, HEAD_OFFSET_LENGTH_FROM_MIDDLE)


SandWormCharacter::SandWormCharacter(
	const WorldTimeManager* time,
	const Terrain* terrain,
	SoundManager* sound,
	RenderableGameObject* sandwormGameObject,
	AnimationSet* animations, 
	float initialX, 
	float initialZ)
:
	_time(time),
	_terrain(terrain),
	_sound(sound),
	_model(sandwormGameObject),
	_animator(animations),
	_currentPos(terrain->getWorldHeightVecFor(initialX, initialZ) + UNDERGROUND_OFFSET),
	_currentFront(glm::vec3(0.0, 0.0, -1.0))
{
	this->_animator.playAnimation(TPOSE_ANIM);
	
	this->_backgroundNoise = this->_sound->playTracked3D(BACKGROUND_RUMBLE_TRACK, true, this->_currentPos - HEAD_OFFSET_VECTOR);
	this->_backgroundNoise->setMinDistance(BACKGROUND_RUMBLE_MIN_DISTANCE);
	this->_backgroundNoise->setVolume(0.75f);

	this->_inFrontNoise = this->_sound->playTracked3D(IN_FRONT_EARTH_BREAKING_TRACK, true, this->_currentPos - HEAD_OFFSET_VECTOR);
	this->_inFrontNoise->setMinDistance(IN_FRONT_EARTH_RIPPING_MIN_DISTANCE);
	//this->_inFrontNoise->setVolume(0.5f);
}

SandWormCharacter::~SandWormCharacter()
{
	this->stopAndClearCurrentSounds();
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

	this->_backgroundNoise->setPosition(SoundManager::convert(this->_currentPos - HEAD_OFFSET_VECTOR));
	this->_inFrontNoise->setPosition(SoundManager::convert(this->_currentPos - HEAD_OFFSET_VECTOR));
}

void SandWormCharacter::draw(Shader& shader)
{
	const std::vector<glm::mat4> transforms = this->_animator.getFinalBoneMatrices();
	this->setupEntityShaderForAnim(shader, transforms);

	this->_model->draw(shader);
	this->clearEntityShaderForAnim(shader);
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
