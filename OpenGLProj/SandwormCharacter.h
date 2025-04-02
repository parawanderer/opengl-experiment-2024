#pragma once
#include "AnimatedEntity.h"
#include "Animator.h"
#include "ParticleSystem.h"
#include "RenderableGameObject.h"
#include "SoundManager.h"
#include "Terrain.h"
#include "WorldTimeManager.h"

struct BehaviourStage
{
	const std::string animationName;
	const bool muteNoise;
};

/**
 * \brief Represents/manages the game state for one of these: https://dune.fandom.com/wiki/Sandworm?so=search
 */
class SandWormCharacter : public AnimatedEntity
{
public:

	enum class MOVEMENT_STATE
	{
		STATIC = 0,
		MOVING = 1
	};

	SandWormCharacter(
		const WorldTimeManager* time, 
		const Terrain* terrain, 
		SoundManager* sound, 
		RenderableGameObject* sandwormGameObject, 
		AnimationSet* animations,
		ParticleSystem* dustParticle1,
		ParticleSystem* dustParticle2,
		float initialX, 
		float initialZ
	);

	void onNewFrame() override;

	void draw(Shader& shader) override;

	void startQuakingEarth();

	void appearAndMoveTowards(const glm::vec3& position);

	void rotateTowards(const glm::vec3& position);

	const glm::vec3& getCurrentPosition() const;
	
private:
	const WorldTimeManager* _time;
	const Terrain* _terrain;
	SoundManager* _sound;

	RenderableGameObject* _model;
	Animator _animator;

	ParticleSystem* _dustParticle1;
	ParticleSystem* _dustParticle2;
	bool _showDust = false;

	float _pitch;
	float _yaw;
	glm::vec3 _currentPos;
	glm::vec3 _currentFront;
	

	MOVEMENT_STATE _movementState = MOVEMENT_STATE::STATIC;

	glm::vec3 _movementStartPos = glm::vec3(0.0f);
	glm::vec3 _movementTarget = glm::vec3(0.0f);
	float _movementStartTime = 0.0f;

	float _yPosOffset = 0.0f;

	float _nextBehaviourChoiceAt = 0.0f;

	float _animationInterpolationStart = 0.0f;
	std::string _animation1 = "";
	std::string _animation2 = "";

	const BehaviourStage* _nextBehaviourStage = nullptr;
	float _nextBehaviourStageAt = -1.0f;

	AudioPlayer _backgroundNoise;
	AudioPlayer _inFrontNoise;

	float _earthquakeInterpolationStart = 0.0f;
	float _inFrontNoiseInterpolationStart = 0.0f;

	glm::vec3 getHeadPosition() const;

	void interpolateSound(const float currentTime);
	void startCreatingPrimaryDestructionNoise();
	void stopAndClearCurrentSounds();

	void updateModelTransform();

	void interpolateMoveState(const float currentTime);
	void playAnimationWithTransition(const std::string& animationName);
	void playAnimation(const std::string& animationName);
	void updateAnimationInterpolation();

	void queueBehaviourStage(const BehaviourStage& behaviourStage, const float executeAfterSeconds);

	void updateParticlePlacement();

	void showDust(bool doShow);

	float getYDifferenceHeadAndBody() const;
};
