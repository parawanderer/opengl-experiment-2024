#include "NomadCharacter.h"

#include <iostream>
#include <glm/ext/matrix_transform.hpp>

#include "GameObjectConstants.h"
#include "WorldMathUtils.h"

constexpr auto WALKING_SPEED_M_PER_SEC = 1.5f;
constexpr auto SMALL_Y_OFFSET = glm::vec3(0.0f, 0.1f, 0.0f);
constexpr auto TIME_BETWEEN_CHOOSING_BEHAVIOURS_SEC = 20;
constexpr auto NOMAD_HEIGHT = 1.9f;

// out of the ones available in its .fbx file...
// the animations are sources from https://www.mixamo.com/#/
// however getting this model compatible with that and also with
// my opengl setup took a good 2-3 days, manually adapting and
// tweaking in blender...
const std::vector<std::string> IDLE_ANIMS = {
	"idle1",
	"idlelookaround1",
	"idlelookarounddown",
	"lookforwarddistance",
	"lookrightquick",
	"lookarounddistance",
	"stretching"
};

const std::string WALKING_ANIM = "walking";
const std::string RUNNING_ANIM_1 = "running1";
const std::string RUNNING_ANIM_2 = "running2";
const std::string RUNNING_LOOK_BEHIND_ANIM = "runninglookbehind";
const std::string RUNNING_FALL_FLAT_ANIM = "fallflat";
const std::string CRAWLING_ANIM = "crawling";
const std::string GETTING_UP_ANIM = "gettingup";
const std::string LOOK_BEHIND_RIGHT_ANIM = "lookright";

// these are reused from player actually
// the footsteps don't actually match the animation but I am too lazy to fix this at the moment
// probably fixable by just speeding up the sound for this character specifically
const std::string NOMAD_SAND_WALKING_TRACK = "006142914-footsteps-walking-sand-01-loop.mp3";
const std::string NOMAD_SAND_RUNNING_TRACK = "098878381-sand-running-footsteps-loop-01.mp3";

struct TalkLineInfo
{
	const std::string line;
	const std::string animationName;
	const float duration;
};


// stuff the character can say
const std::vector<TalkLineInfo> LINES = {
	{"What do you want?", "arguing", 4.0f },
	{"What is it?", "talking_angry_point_righthand", 4.0f },
	{"Can't you see I'm busy?", "talking_dontyougetit", 4.0f },
	{"*Cough*", "", 4.0f }
};

// we can seed the random generator or not. Doesn't really matter for now.
NomadCharacter::NomadCharacter(
	const WorldTimeManager* time, 
	const Terrain* terrain, 
	SoundManager* sound, 
	UICharacterDialogueDisplayManager* dialogueManager,
	SphericalBoxedGameObject* nomadGameObject, 
	AnimationSet* animations, 
	float initialX, 
	float initialZ
) :
	_time(time),
	_terrain(terrain),
	_sound(sound),
	_dialogueManager(dialogueManager),
	_model(nomadGameObject),
	_animator(animations),
	_currentPos(terrain->getWorldHeightVecFor(initialX, initialZ) + SMALL_Y_OFFSET),
	_currentFront(glm::vec3(0.0, 0.0, -1.0))
{}

NomadCharacter::~NomadCharacter()
{
	this->stopAndClearCurrentSound();
}

void NomadCharacter::onNewFrame()
{
	this->_animator.updateAnimation(this->_time->getDeltaTime());

	// decide if want to stand around or walk for the next x 
	const float currentTime = this->_time->getCurrentTime();
	if (currentTime <= this->_nextBehaviourChoiceAt)
	{
		switch(this->_movementState)
		{
		case MOVEMENT_STATE::IDLE:
		case MOVEMENT_STATE::SPEAKING: // nothing to do! No movement will be taking place in either these two states!
			return;
		case MOVEMENT_STATE::WALKING:
			this->interpolateWalkState(currentTime);
			break;
		}
	}
	else
	{
		// choose a new behaviour to do next
		bool wantsToWalkSomewhere = rand() % 2;
		if (!wantsToWalkSomewhere)
		{
			this->defineIdlePlan(currentTime);
			return;
		}
		this->defineWalkPlan(currentTime);
	}

	// TODO: it is possible to do smooth interpolation between the various animations rather than having sharp cut-offs.
	// this would entail interpolating between the two sets of animation matrices.
	// So you "weigh" how much every matrice for every bone contributes to the final result: if t is in range [0,1]
	// then we want the matrice: mWeighed = t * animation1Matrices[i] + (1 - t) * animation2Matrices[i]
	// and we can interpolate t to change over time (e.g. over 1 second we switch from "animation1" to "animation2"
	// it's the same idea as how I am computing the movement, just a few iterations and a bit more state management
	// currently I don't feel like it'll add a lot to this barebones project to add this "smooth" transition.

	this->updateModelTransform();

	if (this->_currentSound != nullptr)
	{
		this->_currentSound->setPosition(SoundManager::convert(this->_currentPos));
	}
}

void NomadCharacter::draw(Shader& shader)
{
	const std::vector<glm::mat4> transforms = this->_animator.getFinalBoneMatrices();
	this->setupEntityShaderForAnim(shader, transforms);

	this->_model->draw(shader);
	
	this->clearEntityShaderForAnim(shader);
}

float NomadCharacter::getRadiusSphericalBoundingBox()
{
	return this->_model->getBoundRadius();
}

glm::vec3 NomadCharacter::getBoundMidPoint()
{
	return this->_model->getWorldMidPoint();
}

void NomadCharacter::saySomething(std::optional<const glm::vec3> lookInDirectionOfCoords)
{
	const TalkLineInfo& action = LINES[rand() % LINES.size()];

	if (!action.animationName.empty())
	{
		// override whatever the current animation is
		this->_movementState = MOVEMENT_STATE::SPEAKING;
		this->_nextBehaviourChoiceAt = this->_time->getCurrentTime() + action.duration;
		this->_animator.playAnimation(action.animationName);
		this->stopAndClearCurrentSound();

		if (lookInDirectionOfCoords.has_value())
		{
			// TODO: currently characters can't really look up or down, but it might be nice to add this at some point
			glm::vec3 newDirection = lookInDirectionOfCoords.value() - this->_currentPos;
			newDirection.y = 0.0f;
			newDirection = glm::normalize(newDirection);

			this->_currentFront = newDirection;

			this->_pitch = 0.0f;
			this->_yaw = glm::degrees(acos(glm::dot(this->_currentFront, glm::vec3(-1.0, 0.0, 0.0)))) + 180.0f;
			if (glm::dot(this->_currentFront, glm::vec3(0.0, 0.0, 1.0)) > 0.0f) 
				this->_yaw = 360.0f - this->_yaw;
			this->updateModelTransform(); 
		}
	}

	this->_dialogueManager->requestDialogue(NOMAD, action.line, action.duration, this);
}

glm::vec3 NomadCharacter::getCurrentPosition() const
{
	return this->_currentPos;
}

void NomadCharacter::interpolateWalkState(const float currentTime)
{
	float t = (currentTime - this->_movementStartTime) / (this->_nextBehaviourChoiceAt - this->_movementStartTime); // t = range [0, 1]
	// linear interpolation
	const glm::vec3 pos = (1 - t) * this->_movementStartPos + t * this->_movementTarget; // at t = 0, completely @ start, otherwise completely at end
	this->_currentPos = this->_terrain->getWorldHeightVecFor(pos.x, pos.z) + SMALL_Y_OFFSET;
}

void NomadCharacter::defineWalkPlan(const float currentTime)
{
	// **how far** the character wants to walk (in world units)
	const float desiredWalkDistance = rand() % 20 + 5;
	const float angle = rand() % 360 + 1 - 180; // (angle between -180 and 180 degrees)

	this->_yaw += angle;
	if (this->_yaw < 0) this->_yaw += 360.0f;
	if (this->_yaw > 360.0f) this->_yaw -= 360.0f;

	this->_currentFront = WorldMathUtils::computeDirection(this->_pitch, this->_yaw);

	this->_movementStartPos = this->_currentPos;
	this->_movementTarget = this->_currentPos + (this->_currentFront) * desiredWalkDistance; // interpolate between them? (ignore "y" dimension -> get it from the height map)
	this->_movementStartTime = currentTime;
	this->_nextBehaviourChoiceAt = currentTime + (desiredWalkDistance / WALKING_SPEED_M_PER_SEC); // (seconds) (character will walk desiredWalkDistance in this amount of seconds if he walks at WALKING_SPEED_M_PER_SEC)
	this->_movementState = MOVEMENT_STATE::WALKING;

	this->_animator.playAnimation(WALKING_ANIM);

	this->stopAndClearCurrentSound();
	this->_currentSound = this->_sound->playTracked3D(NOMAD_SAND_WALKING_TRACK, true, this->_currentPos);
}

void NomadCharacter::defineIdlePlan(const float currentTime)
{
	this->_nextBehaviourChoiceAt = currentTime + (rand() % TIME_BETWEEN_CHOOSING_BEHAVIOURS_SEC + 1); // <- seconds
	this->_movementState = MOVEMENT_STATE::IDLE; // just stand still
	this->_animator.playAnimation(IDLE_ANIMS[rand() % IDLE_ANIMS.size()]);
	this->stopAndClearCurrentSound();
}

void NomadCharacter::updateModelTransform()
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->_currentPos); // place character at current position
	model = glm::rotate(model, -glm::radians(this->_yaw) + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // orient character along his movement direction
	model = glm::scale(model, glm::vec3(0.01f));
	this->_model->setModelTransform(model);
}

void NomadCharacter::stopAndClearCurrentSound()
{
	if (this->_currentSound != nullptr)
	{
		this->_currentSound->stop();
		this->_currentSound->drop();
		this->_currentSound = nullptr;
	}
}
