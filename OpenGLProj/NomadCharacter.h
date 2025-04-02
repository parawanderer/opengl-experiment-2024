#ifndef NOMADCHARACTER_MINE_H
#define NOMADCHARACTER_MINE_H

#include "AnimatedEntity.h"
#include "Animator.h"
#include "RenderableGameObject.h"
#include "SoundManager.h"
#include "Terrain.h"
#include "WorldTimeManager.h"

/**
 * \brief Represents a "Nomad" NPC character in the game. Manages a single Nomad's state
 */
class NomadCharacter : public AnimatedEntity
{
public:
	enum class MOVEMENT_STATE
	{
		IDLE = 0,
		WALKING = 1
	};

	NomadCharacter(const WorldTimeManager* time, const Terrain* terrain, SoundManager* sound, RenderableGameObject* nomadGameObject, AnimationSet* animations, float initialX, float initialZ);

	~NomadCharacter() override;

	void onNewFrame() override;
	void draw(Shader& shader) override;
private:
	const WorldTimeManager* _time;
	const Terrain* _terrain;

	SoundManager* _sound;

	RenderableGameObject* _model;
	Animator _animator;

	glm::vec3 _currentPos;
	glm::vec3 _currentFront;
	float _pitch = 0.0f;
	float _yaw = -90.0f;

	MOVEMENT_STATE _movementState = MOVEMENT_STATE::IDLE;
	irrklang::ISound* _currentSound = nullptr;

	glm::vec3 _movementStartPos = glm::vec3(0.0f);
	glm::vec3 _movementTarget = glm::vec3(0.0f);
	float _movementStartTime = 0.0f;

	float _nextBehaviourChoiceAt = 0.0f;

	void interpolateWalkState(float currentTime);
	void defineWalkPlan(const float currentTime);

	void stopAndClearCurrentSound();
};

#endif