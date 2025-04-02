#pragma once
#include "AnimatedEntity.h"
#include "Animator.h"
#include "RenderableGameObject.h"
#include "Terrain.h"
#include "WorldTimeManager.h"

class SandWormCharacter : public AnimatedEntity
{
public:

	enum class MOVEMENT_STATE
	{
		UNDERGROUND = 0,
		ABOVE_GROUND = 1
	};

	SandWormCharacter(WorldTimeManager* time, Terrain* terrain, RenderableGameObject* sandwormGameObject, AnimationManager* animations, float initialX, float initialZ);


	void onNewFrame() override;

	void draw(Shader& shader) override;

private:
	WorldTimeManager* _time;
	Terrain* _terrain;

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
