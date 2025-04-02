#ifndef ORNITHOPTERCHARACTER_MINE_H
#define ORNITHOPTERCHARACTER_MINE_H
#include "AnimatedEntity.h"
#include "RenderableGameObject.h"
#include "WorldTimeManager.h"

/**
 * \brief Represents one of these: https://dune.fandom.com/wiki/Ornithopter
 */
class OrnithopterCharacter : public AnimatedEntity
{
public:

	OrnithopterCharacter(const WorldTimeManager* time, RenderableGameObject* ornithropterObject);

	void onNewFrame() override;
	void draw(Shader& shader) override;

private:
	const WorldTimeManager* _time;

	RenderableGameObject* _model;

	glm::vec3 _currentPos;
	glm::vec3 _currentFront;
	float _yaw = -90.0f;
};

#endif