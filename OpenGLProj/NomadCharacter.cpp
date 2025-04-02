#include "NomadCharacter.h"

#include <iostream>
#include <glm/ext/matrix_transform.hpp>

#include "WorldMathUtils.h"

#define WALKING_SPEED_M_PER_SEC 1.42f
#define SMALL_Y_OFFSET  glm::vec3(0.0f, 0.1f, 0.0f)
#define TIME_BETWEEN_CHOOSING_BEHAVIOURS_SEC 20 


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
	"lookarounddistance"
};

const std::string WALKING_ANIM = "walking";
const std::string RUNNING_ANIM_1 = "running1";
const std::string RUNNING_ANIM_2 = "running2";
const std::string RUNNING_LOOK_BEHIND_ANIM = "runninglookbehind";
const std::string RUNNING_FALL_FLAT_ANIM = "fallflat";
const std::string CRAWLING_ANIM = "crawling";
const std::string GETTING_UP_ANIM = "gettingup";
const std::string LOOK_BEHIND_RIGHT_ANIM = "lookright";

// we can seed the random generator or not. Doesn't really matter for now.
NomadCharacter::NomadCharacter(WorldTimeManager* time, Terrain* terrain, RenderableGameObject* nomadGameObject, Animator* animator, float initialX, float initialZ) :
	_time(time),
	_terrain(terrain),
	_nomadModel(nomadGameObject),
	_animator(animator),
	_currentPos(terrain->getWorldHeightVecFor(initialX, initialZ) + SMALL_Y_OFFSET),
	_currentFront(glm::vec3(0.0, 0.0, -1.0))
{}

void NomadCharacter::onNewFrame()
{
	// decide if want to stand around or walk for the next x 
	const float currentTime = this->_time->getCurrentTime();
	if (currentTime <= this->_nextBehaviourChoiceAt)
	{
		switch(this->_movementState)
		{
		case MOVEMENT_STATE::IDLE:
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
		//std::cout << "wants to walk?: " << wantsToWalkSomewhere << "\n";
		if (!wantsToWalkSomewhere)
		{
			this->_nextBehaviourChoiceAt = currentTime + (rand() % TIME_BETWEEN_CHOOSING_BEHAVIOURS_SEC + 1); // <- seconds
			this->_movementState = MOVEMENT_STATE::IDLE; // just stand still
			this->_animator->playAnimation(IDLE_ANIMS[rand() % IDLE_ANIMS.size()]);
			return;
		}
		this->defineWalkPlan(currentTime);
		this->_animator->playAnimation("walking");
	}

	// TODO: it is possible to do smooth interpolation between the various animations rather than having sharp cut-offs

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->_currentPos); // place character at current position
	model = glm::rotate(model, -glm::radians(this->_yaw) + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // orient character along his movement direction
	//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate the model into upward position
	model = glm::scale(model, glm::vec3(0.01f));
	this->_nomadModel->setModelTransform(model);
}

void NomadCharacter::draw(Shader& shader)
{
	glGetError();
	shader.setBool("doAnimate", true);
	glGetError();
	const std::vector<glm::mat4> transforms = this->_animator->getFinalBoneMatrices();
	glGetError();
	for (int i = 0; i < transforms.size(); ++i)
	{
		shader.setMat4("finalBoneMatrices[" + std::to_string(i) + "]", transforms[i]);
		glGetError();
	}

	this->_nomadModel->draw(shader);
	glGetError();
	shader.setBool("doAnimate", false);
	glGetError();
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
}
