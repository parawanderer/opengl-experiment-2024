#ifndef SANDWORMCHARACTER_MINE_H
#define SANDWORMCHARACTER_MINE_H

#include "Animator.h"
#include "GenericAnimatedCharacter.h"
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
class SandWormCharacter : public GenericAnimatedCharacter
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

	~SandWormCharacter() override = default;

	void onNewFrame() override;

	void startQuakingEarth();

	void appearAndMoveTowards(const glm::vec3& position);

protected:
	void updateModelTransform() override;
	
private:
	const Terrain* _terrain;
	SoundManager* _sound;

	RenderableGameObject* _model;

	ParticleSystem* _dustParticle1;
	ParticleSystem* _dustParticle2;
	bool _showDust = false;

	MOVEMENT_STATE _movementState = MOVEMENT_STATE::STATIC;
	glm::vec3 _movementStartPos = glm::vec3(0.0f);
	glm::vec3 _movementTarget = glm::vec3(0.0f);
	float _movementStartTime = 0.0f;

	float _yPosOffset = 0.0f;

	float _movementEndTime = 0.0f;
	const BehaviourStage* _nextBehaviourStage = nullptr;
	float _nextBehaviourStageAt = -1.0f;

	AudioPlayer _backgroundNoise;
	float _bgNoiseInterpolationStart = 0.0f;

	AudioPlayer _foregroundNoise;
	float _fgInterpolationStart = 0.0f;

	glm::vec3 getHeadPosition() const;

	void interpolateSound(const float currentTime);

	static void interpolateSpecificSound(const float currentTime, AudioPlayer& soundToInterpolate, const float interpolationStartTime, const float interpolationDuration);

	void startCreatingPrimaryDestructionNoise();

	void interpolateMoveState(const float currentTime);

	void enqueueBehaviourStage(const BehaviourStage& behaviourStage, const float executeAfterSeconds);
	bool hasEnqueuedBehaviourStagePending(const float currentTime) const;

	void updateParticlePlacement();

	void showDust(bool doShow);

	float getYDifferenceHeadAndBody() const;
};

#endif