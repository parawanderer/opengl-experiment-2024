#include "NomadCharacter.h"

#include <iostream>
#include <glm/ext/matrix_transform.hpp>

#include "GameObjectConstants.h"
#include "InterpolationMathUtil.h"
#include "WorldMathUtils.h"


constexpr auto WALKING_SPEED_M_PER_SEC = 1.5f;
constexpr auto RUNNING_SPEED_M_PER_SEC = 6.0f;
constexpr auto SMALL_Y_OFFSET = glm::vec3(0.0f, 0.1f, 0.0f);
constexpr auto TIME_BETWEEN_CHOOSING_BEHAVIOURS_SEC = 20;
constexpr auto NOMAD_HEIGHT = 1.9f;
constexpr auto NOMAD_HEIGHT_OFFSET = glm::vec3(0.0f, NOMAD_HEIGHT, 0.0f);
constexpr auto ANIMATION_TRANSITION_TIME = 0.2f;
constexpr auto VOICELINE_MIN_DISTANCE = 3.0f;

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

const std::string VOICELINES_BASE_FOLDER = "nomad/";



// stuff the character can say
const std::vector<TalkLineInfo> LINES = {
	{
		.line ="What do you want?",
		.animationName = "arguing",
		.duration = 1.5f,
		.voicelineAudio = "what_do_you_want.mp3"
	},
	{
		.line = "What is it?",
		.animationName = "talking_angry_point_righthand",
		.duration = 1.5f,
		.voicelineAudio = "what_is_it.mp3"
	},
	{
		.line = "Can't you see I'm busy?",
		.animationName = "talking_dontyougetit",
		.duration = 2.0f,
		.voicelineAudio = "cant_you_see_im_busy.mp3"
	},
	{
		.line = "*Cough*",
		.animationName = "",
		.duration = 1.5f,
		.voicelineAudio = "cough.mp3"
	}
};

int thumperLinesIndex = 0; // TODO: remove, obviously wouldn't work for multiple nomad instances
const std::vector<TalkLineInfo> THUMPER_LINES = {
	{
		.line = "Huh, where did you get that?",
		.animationName = "talking_angry_right_hand",
		.duration = 2.5f,
		.voicelineAudio = "huh_where_did_you_get_that.mp3"
	},
	{
		.line = "Say, what is that thing called again?",
		.animationName = "idle1",
		.duration = 2.5f,
		.voicelineAudio = "say_what_is_that_thing_called_again.mp3"
	}
};

int lines2Index = 0; // TODO: remove, obviously wouldn't work for multiple nomad instances
const std::vector<TalkLineInfo> LINES_2 = {
	{
		.line = "Listen, you seem good at finding things. Have you seen a small black box around here?",
		.animationName = "idle1",
		.duration = 5.0f,
		.voicelineAudio = "listen_you_seem_good_at_finding_things_have_you_seen_a_small_black_box_around_here.mp3"
	},
	{
		.line = "Guess you didn't find that box, eh?",
		.animationName = "talking_righthand_lower",
		.duration = 2.5f,
		.voicelineAudio = "guess_you_didnt_find_that_box_eh.mp3"
	}
};

const TalkLineInfo WHAT_IS_THAT = {
	.line = "What is that?!",
	.animationName = "idlelookaround1",
	.duration = 2.5f,
	.voicelineAudio = "what_is_that.mp3"
};

const TalkLineInfo HOLY_MOTHER_OF = {
	.line = "Holy mother of-?!",
	.animationName = "spooked",
	.duration = 2.5f,
	.voicelineAudio = "holy_mother_of.mp3"
};

const TalkLineInfo WILHELM_SCREAM = {
	.line = "AAAAAAAA!!!!",
	.animationName = "crawling2",
	.duration = 2.5f,
	.voicelineAudio = "wilhelm_scream.mp3",
};


float tmpNextBehaviourChoiceOverride = -1.0f; // TODO: to be removed, probably not really needed as I keep working on this

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
	this->stopAndClearCurrentVoiceline();
}

void NomadCharacter::onNewFrame()
{
	this->updateAnimationInterpolation();


	// decide if want to stand around or walk for the next x 
	const float currentTime = this->_time->getCurrentTime();
	if (tmpNextBehaviourChoiceOverride > -1.0f && currentTime >= tmpNextBehaviourChoiceOverride)
	{
		// TODO: temporary. remove. Not sure if we even need anything like this later down the line...
		this->playAnimationWithTransition(RUNNING_ANIM_2);
		this->stopAndClearCurrentSound();
		this->_currentSound = this->_sound->playTracked3D(NOMAD_SAND_RUNNING_TRACK, true, this->_currentPos);
		tmpNextBehaviourChoiceOverride = -1.0f;
	}
	else if (currentTime < this->_nextBehaviourChoiceAt)
	{
		switch(this->_movementState)
		{
		case MOVEMENT_STATE::IDLE:
		case MOVEMENT_STATE::SPEAKING: // nothing to do! No movement will be taking place in either these two states!
			return;
		case MOVEMENT_STATE::WALKING:
		case MOVEMENT_STATE::RUNNING:
			this->interpolateMoveState(currentTime);
			break;
		}
	}
	else
	{
		this->decideNextBehaviour(currentTime);
	}

	this->updateModelTransform();

	if (this->_currentSound != nullptr) this->_currentSound->setPosition(SoundManager::convert(this->_currentPos));
	if (this->_currentVoiceLine != nullptr) this->_currentVoiceLine->setPosition(SoundManager::convert(this->_currentPos + NOMAD_HEIGHT_OFFSET));
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
		this->doDialogueAnimation(action.animationName, lookInDirectionOfCoords, action.duration);
	}

	this->_dialogueManager->requestDialogue(NOMAD, action.line, action.duration, this);
	this->sayVoiceLine(action.voicelineAudio);
}

void NomadCharacter::commentOnThumper(const glm::vec3 lookInDirectionOfCoords)
{
	const TalkLineInfo& action = THUMPER_LINES[thumperLinesIndex];
	this->doDialogueAnimation(action.animationName, lookInDirectionOfCoords, action.duration);
	this->_dialogueManager->requestDialogue(NOMAD, action.line, action.duration, this);
	this->sayVoiceLine(action.voicelineAudio);

	thumperLinesIndex = std::min((thumperLinesIndex + 1), (int)THUMPER_LINES.size() - 1);
}

void NomadCharacter::askAboutLostItem(const glm::vec3 lookInDirectionOfCoords)
{
	const TalkLineInfo& action = LINES_2[lines2Index];
	this->doDialogueAnimation(action.animationName, lookInDirectionOfCoords, action.duration);
	this->_dialogueManager->requestDialogue(NOMAD, action.line, action.duration, this);
	this->sayVoiceLine(action.voicelineAudio);

	lines2Index = std::min((lines2Index + 1), (int)LINES_2.size() - 1);
}

void NomadCharacter::runOverAndWhatIsThat(const glm::vec3 runOverTo, const float offsetByUnits)
{
	// RUN OVER
	this->runOverTo(runOverTo, offsetByUnits);

	// WHEN FINISHED RUNNING THEN TRIGGER THE "WHAT IS THAT" LINE!
	this->registerVoiceLineUponCompletionCurrentBehaviour(WHAT_IS_THAT);
}

void NomadCharacter::reactToSandworm()
{
	this->doDialogueAnimation(HOLY_MOTHER_OF.animationName, std::nullopt, HOLY_MOTHER_OF.duration + 999999.0f);
	this->_dialogueManager->requestDialogue(NOMAD, HOLY_MOTHER_OF.line, HOLY_MOTHER_OF.duration, this);
	this->sayVoiceLine(HOLY_MOTHER_OF.voicelineAudio);
}

void NomadCharacter::playLookBackAnimWhileRunning()
{
	this->playAnimationWithTransition(RUNNING_LOOK_BEHIND_ANIM);
	this->stopAndClearCurrentSound();
	this->_currentSound = this->_sound->playTracked3D(NOMAD_SAND_WALKING_TRACK, true, this->_currentPos);
	tmpNextBehaviourChoiceOverride = this->_time->getCurrentTime() + 2.0f;
}

void NomadCharacter::runStopByFallingAndStayDown()
{
	// fall right now
	this->doDialogueAnimation(RUNNING_FALL_FLAT_ANIM, std::nullopt, 2.52f - ANIMATION_TRANSITION_TIME);

	// then do this:
	this->registerVoiceLineUponCompletionCurrentBehaviour(WILHELM_SCREAM);
}

glm::vec3 NomadCharacter::getCurrentPosition() const
{
	return this->_currentPos;
}

void NomadCharacter::runOverTo(const glm::vec3 runOverTo, const float offsetByUnits, bool doFunnyRun)
{
	const float currentTime = this->_time->getCurrentTime();
	this->rotateTowards(runOverTo);

	const glm::vec3 runTarget = runOverTo - (offsetByUnits * this->_currentFront);

	this->_movementStartPos = this->_currentPos;
	this->_movementTarget = this->_terrain->getWorldHeightVecFor(runTarget.x, runTarget.z) + SMALL_Y_OFFSET;
	this->_movementStartTime = currentTime;
	const float runDistance = glm::distance(this->_currentPos, runTarget);
	this->_nextBehaviourChoiceAt = currentTime + (runDistance / RUNNING_SPEED_M_PER_SEC) + ANIMATION_TRANSITION_TIME;

	this->_movementState = MOVEMENT_STATE::RUNNING;

	this->playAnimationWithTransition(doFunnyRun ? RUNNING_ANIM_2 : RUNNING_ANIM_1);
	this->stopAndClearCurrentSound();
	this->_currentSound = this->_sound->playTracked3D(NOMAD_SAND_RUNNING_TRACK, true, this->_currentPos);
}

const glm::vec3& NomadCharacter::getCurrentPosition()
{
	return this->_currentPos;
}

void NomadCharacter::decideNextBehaviour(const float currentTime)
{
	// choose a new behaviour to do next
	if (this->_voiceLineUponCompletionBehaviour != nullptr)
	{
		this->performQueuedDialogue();
		return;
	}

	bool wantsToWalkSomewhere = rand() % 2;
	if (!wantsToWalkSomewhere)
	{
		this->defineIdlePlan(currentTime);
		return;
	}
	this->defineWalkPlan(currentTime);
}

void NomadCharacter::interpolateMoveState(const float currentTime)
{
	float t = (currentTime - this->_movementStartTime) / (this->_nextBehaviourChoiceAt - this->_movementStartTime); // t = range [0, 1]
	// linear interpolation
	const glm::vec3 pos = (1 - t) * this->_movementStartPos + t * this->_movementTarget; // at t = 0, completely @ start, otherwise completely at end
	this->_currentPos = this->_terrain->getWorldHeightVecFor(pos.x, pos.z) + SMALL_Y_OFFSET;
}

void NomadCharacter::defineWalkPlan(const float currentTime)
{
	// **how far** the character wants to walk (in world units)
	const float desiredWalkDistance = rand() % 10 + 3.0f;
	const float angle = rand() % 360 + 1 - 180; // (angle between -180 and 180 degrees)

	this->_yaw += angle;
	if (this->_yaw < 0) this->_yaw += 360.0f;
	if (this->_yaw > 360.0f) this->_yaw -= 360.0f;

	this->_currentFront = WorldMathUtils::computeDirection(this->_pitch, this->_yaw);

	this->_movementStartPos = this->_currentPos;
	this->_movementTarget = this->_currentPos + (this->_currentFront) * desiredWalkDistance; // interpolate between them? (ignore "y" dimension -> get it from the height map)
	this->_movementStartTime = currentTime;
	this->_nextBehaviourChoiceAt = currentTime + (desiredWalkDistance / WALKING_SPEED_M_PER_SEC) + ANIMATION_TRANSITION_TIME; // (seconds) (character will walk desiredWalkDistance in this amount of seconds if he walks at WALKING_SPEED_M_PER_SEC)
	this->_movementState = MOVEMENT_STATE::WALKING;

	this->playAnimationWithTransition(WALKING_ANIM);

	this->stopAndClearCurrentSound();
	this->_currentSound = this->_sound->playTracked3D(NOMAD_SAND_WALKING_TRACK, true, this->_currentPos);
}

void NomadCharacter::defineIdlePlan(const float currentTime)
{
	this->_nextBehaviourChoiceAt = currentTime + (rand() % TIME_BETWEEN_CHOOSING_BEHAVIOURS_SEC + 1) + ANIMATION_TRANSITION_TIME; // <- seconds
	this->_movementState = MOVEMENT_STATE::IDLE; // just stand still
	this->playAnimationWithTransition(IDLE_ANIMS[rand() % IDLE_ANIMS.size()]);
	this->stopAndClearCurrentSound();
}

void NomadCharacter::performQueuedDialogue()
{
	this->doDialogueAnimation(
		this->_voiceLineUponCompletionBehaviour->animationName,
		std::nullopt,
		this->_voiceLineUponCompletionBehaviour->duration + 999999.0f // TODO: remove this large number as this is temporary to provide a "cinematic" experience only,
	);
	this->_dialogueManager->requestDialogue(NOMAD, this->_voiceLineUponCompletionBehaviour->line, this->_voiceLineUponCompletionBehaviour->duration, this);
	this->sayVoiceLine(this->_voiceLineUponCompletionBehaviour->voicelineAudio);


	this->_voiceLineUponCompletionBehaviour = nullptr;
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

void NomadCharacter::stopAndClearCurrentVoiceline()
{
	if (this->_currentVoiceLine != nullptr)
	{
		this->_currentVoiceLine->stop();
		this->_currentVoiceLine->drop();
		this->_currentVoiceLine = nullptr;
	}
}

void NomadCharacter::sayVoiceLine(const std::string& voicelineFileName)
{
	this->stopAndClearCurrentVoiceline();
	this->_currentVoiceLine = this->_sound->playTracked3D(VOICELINES_BASE_FOLDER + voicelineFileName, false, this->_currentPos + NOMAD_HEIGHT_OFFSET);
	this->_currentVoiceLine->setMinDistance(VOICELINE_MIN_DISTANCE);
}

void NomadCharacter::doDialogueAnimation(const std::string& animationName, std::optional<const glm::vec3> lookInDirectionOfCoords, float playAnimationForSecs, bool smoothTransition)
{
	if (animationName.empty()) throw std::exception("Empty animation provided");

	// override whatever the current animation is
	this->_movementState = MOVEMENT_STATE::SPEAKING;
	this->_nextBehaviourChoiceAt = this->_time->getCurrentTime() + playAnimationForSecs /*+ ANIMATION_TRANSITION_TIME*/;
	if (smoothTransition)
	{
		this->playAnimationWithTransition(animationName);
	}
	else
	{
		this->playAnimation(animationName);
	}
	
	this->stopAndClearCurrentSound();

	if (lookInDirectionOfCoords.has_value())
	{
		this->rotateTowards(lookInDirectionOfCoords.value());
	}
}

void NomadCharacter::playAnimation(const std::string& animationName)
{
	this->_animator.clearAnimation2();
	this->_animator.playAnimation(animationName);
	this->_animation1 = animationName;
}

void NomadCharacter::playAnimationWithTransition(const std::string& animationName)
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

void NomadCharacter::updateAnimationInterpolation()
{
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

void NomadCharacter::rotateTowards(const glm::vec3& position)
{
	// TODO: currently characters can't really look up or down, but it might be nice to add this at some point
	glm::vec3 newDirection = position - this->_currentPos;
	newDirection.y = 0.0f;
	newDirection = glm::normalize(newDirection);

	this->_currentFront = newDirection;

	this->_pitch = 0.0f;
	this->_yaw = glm::degrees(acos(glm::dot(this->_currentFront, glm::vec3(-1.0, 0.0, 0.0)))) + 180.0f;
	if (glm::dot(this->_currentFront, glm::vec3(0.0, 0.0, 1.0)) > 0.0f)
		this->_yaw = 360.0f - this->_yaw;
	this->updateModelTransform();
}

void NomadCharacter::registerVoiceLineUponCompletionCurrentBehaviour(const TalkLineInfo& voiceLine)
{
	this->_voiceLineUponCompletionBehaviour = &voiceLine;
}
