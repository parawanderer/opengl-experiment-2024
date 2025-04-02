#pragma once
#include "AnimatedEntity.h"
#include "Animator.h"
#include "RenderableGameObject.h"
#include "Terrain.h"
#include "WorldTimeManager.h"

/**
 * \brief Represents/manages the game state for one of these: https://dune.fandom.com/wiki/Sandworm?so=search
 */
class SandWormCharacter : public AnimatedEntity
{
public:

	enum class MOVEMENT_STATE
	{
		UNDERGROUND = 0,
		ABOVE_GROUND = 1
	};

	SandWormCharacter(const WorldTimeManager* time, const Terrain* terrain, RenderableGameObject* sandwormGameObject, AnimationSet* animations, float initialX, float initialZ);


	void onNewFrame() override;

	void draw(Shader& shader) override;

private:
	const WorldTimeManager* _time;
	const Terrain* _terrain;

	RenderableGameObject* _model;
	Animator _animator;

	glm::vec3 _currentPos;
	glm::vec3 _currentFront;
	float _pitch = 0.0f;
	float _yaw = -90.0f;

	MOVEMENT_STATE _movementState = MOVEMENT_STATE::UNDERGROUND;

	glm::vec3 _movementStartPos = glm::vec3(0.0f);
	glm::vec3 _movementTarget = glm::vec3(0.0f);
	float _movementStartTime = 0.0f;

	float _nextBehaviourChoiceAt = 0.0f;
};
