#ifndef NOMADCHARACTER_MINE_H
#define NOMADCHARACTER_MINE_H
#include "AnimatedEntity.h"
#include "Animator.h"
#include "RenderableGameObject.h"
#include "Terrain.h"
#include "WorldTimeManager.h"

class NomadCharacter : public AnimatedEntity
{
public:
	enum class MOVEMENT_STATE
	{
		IDLE = 0,
		WALKING = 1
	};

	//NomadCharacter(WorldTimeManager* time, Terrain* terrain, RenderableGameObject* nomadGameObject, Animator* animator, float initialX, float initialZ);
	NomadCharacter(WorldTimeManager* time, Terrain* terrain, RenderableGameObject* nomadGameObject, AnimationManager* animations, float initialX, float initialZ);

	void onNewFrame() override;
	void draw(Shader& shader) override;
private:
	WorldTimeManager* _time;
	Terrain* _terrain;

	RenderableGameObject* _nomadModel;
	Animator _animator;

	glm::vec3 _currentPos;
	glm::vec3 _currentFront;
	float _pitch = 0.0f;
	float _yaw = -90.0f;

	MOVEMENT_STATE _movementState = MOVEMENT_STATE::IDLE;
	glm::vec3 _movementStartPos = glm::vec3(0.0f);
	glm::vec3 _movementTarget = glm::vec3(0.0f);
	float _movementStartTime = 0.0f;

	float _nextBehaviourChoiceAt = 0.0f;

	void interpolateWalkState(float currentTime);
	void defineWalkPlan(const float currentTime);
};

#endif