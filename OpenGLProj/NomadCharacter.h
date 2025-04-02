#ifndef NOMADCHARACTER_MINE_H
#define NOMADCHARACTER_MINE_H

#include "AnimatedEntity.h"
#include "Animator.h"
#include "DialogueRequester.h"
#include "RenderableGameObject.h"
#include "SphericalBoundingBoxedEntity.h"
#include "SoundManager.h"
#include "SphericalBoxedGameObject.h"
#include "Terrain.h"
#include "UICharacterDialogueDisplayManager.h"
#include "WorldTimeManager.h"


struct TalkLineInfo
{
	const std::string line;
	const std::string animationName;
	const float duration;
	const std::string voicelineAudio;
};

/**
 * \brief Represents a "Nomad" NPC character in the game. Manages a single Nomad's state
 */
class NomadCharacter : public AnimatedEntity, public SphericalBoundingBoxedEntity, public DialogueRequester
{
public:
	enum class MOVEMENT_STATE
	{
		IDLE = 0,
		WALKING = 1,
		SPEAKING = 2,
		RUNNING = 3
	};

	NomadCharacter(
		const WorldTimeManager* time, 
		const Terrain* terrain, 
		SoundManager* sound, 
		UICharacterDialogueDisplayManager* dialogueManager, 
		SphericalBoxedGameObject* nomadGameObject, 
		AnimationSet* animations, 
		float initialX, 
		float initialZ);

	void onNewFrame() override;
	void draw(Shader& shader) override;

	float getRadiusSphericalBoundingBox() override;
	glm::vec3 getBoundMidPoint() override;

	/**
	 * \brief Say some generic line of dialogue
	 *
	 * \param lookInDirectionOfCoords		if this is provided and if the character chooses to say something towards the target,
	 *										the character will turn towards the coordinates given here when saying their line.
	 */
	void saySomething(std::optional<const glm::vec3> lookInDirectionOfCoords = std::nullopt);

	/**
	 * \brief trigger a comment on the player holding a thumper item specifically (TODO: remove this and make a better dialogue system)
	 */
	void commentOnThumper(const glm::vec3 lookInDirectionOfCoords);

	/**
	 * \brief trigger a comment on the player seeing another lost item (TODO: remove this and make a better dialogue system)
	 */
	void askAboutLostItem(const glm::vec3 lookInDirectionOfCoords);

	glm::vec3 getCurrentPosition() const override;

	/**
	 * \brief Makes the character run over to the position.
	 * \param runOverTo				Position to run over to. y component gets ignored as the y position will be taken from the world map.
	 * \param offsetByUnits			Make the character stop this many meters before reaching the position.
	 * \param doFunnyRun			Whether to use the funnier run animation
	 */
	void runOverTo(const glm::vec3 runOverTo, const float offsetByUnits = 0.0f, const bool doFunnyRun = false);

	const glm::vec3& getCurrentPosition();


	// TODO: remove this:

	/**
	 * \brief Temporary! TODO: remove this after assignment
	 */
	void runOverAndWhatIsThat(const glm::vec3 runOverTo, const float offsetByUnits = 0.0f);

	/**
	 * \brief Temporary! TODO: remove this after assignment
	 */
	void reactToSandworm();

	/**
	 * \brief Temporary! TODO: remove this after assignment
	 */
	void playLookBackAnimWhileRunning();

	/**
	 * \brief Temporary! TODO: remove this after assignment
	 */
	void runStopByFallingAndStayDown();

private:
	const WorldTimeManager* _time;
	const Terrain* _terrain;

	SoundManager* _sound;

	UICharacterDialogueDisplayManager* _dialogueManager;

	SphericalBoxedGameObject* _model;
	Animator _animator;

	float _animationInterpolationStart = 0.0f;
	std::string _animation1 = "";
	std::string _animation2 = "";

	glm::vec3 _currentPos;
	glm::vec3 _currentFront;
	float _pitch = 0.0f;
	float _yaw = -90.0f;

	MOVEMENT_STATE _movementState = MOVEMENT_STATE::IDLE;

	AudioPlayer _currentSound;
	AudioPlayer _currentVoiceLine;

	glm::vec3 _movementStartPos = glm::vec3(0.0f);
	glm::vec3 _movementTarget = glm::vec3(0.0f);
	float _movementStartTime = 0.0f;

	float _nextBehaviourChoiceAt = 0.0f;
	const TalkLineInfo* _voiceLineUponCompletionBehaviour = nullptr;

	void decideNextBehaviour(const float currentTime);

	void interpolateMoveState(float currentTime);
	void defineWalkPlan(const float currentTime);
	void defineIdlePlan(const float currentTime);
	void performQueuedDialogue();
	void updateModelTransform();

	void sayVoiceLine(const std::string& voicelineFileName);

	/**
	 * \brief Makes character abort current behaviour and say the dialogue.
	 * \param animationName					Name of animation to perform while doing the dialogue
	 * \param lookInDirectionOfCoords		If this is provided, then the character will turn in this direction while performing the animation. If not, the character retains their current direction.
	 * \param playAnimationForSecs			How long the animation will be performed for
	 */
	void doDialogueAnimation(const std::string& animationName, std::optional<const glm::vec3> lookInDirectionOfCoords, float playAnimationForSecs, bool smoothTransition = true);
	void playAnimation(const std::string& animationName);
	void playAnimationWithTransition(const std::string& animationName);
	void updateAnimationInterpolation();

	void rotateTowards(const glm::vec3& position);

	void registerVoiceLineUponCompletionCurrentBehaviour(const TalkLineInfo& voiceLine);
};

#endif