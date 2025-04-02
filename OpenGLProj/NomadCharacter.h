#ifndef NOMADCHARACTER_MINE_H
#define NOMADCHARACTER_MINE_H
#include "RenderableGameObject.h"
#include "Terrain.h"
#include "WorldTimeManager.h"

class NomadCharacter
{
public:
	enum class MOVEMENT_STATE
	{
		IDLE = 0,
		WALKING = 1
	};

	NomadCharacter(WorldTimeManager* time, Terrain* terrain, RenderableGameObject* nomadGameObject, float initialX, float initialZ);

	void onNewFrame();

	void draw(Shader& shader);
private:
	WorldTimeManager* _time;
	Terrain* _terrain;

	RenderableGameObject* _nomadModel;

	glm::vec3 _currentPos;
	glm::vec3 _currentFront;
	float _pitch = 0.0f;
	float _yaw = -90.0f;

	MOVEMENT_STATE _movementState = MOVEMENT_STATE::IDLE;
	glm::vec3 _movementStartPos = glm::vec3(0.0f);
	glm::vec3 _movementTarget = glm::vec3(0.0f);
	float _movementStartTime = 0.0f;

	float _nextBehaviourChoiceAt = 0.0f;
};

#endif