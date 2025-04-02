#ifndef ORNITHOPTERCHARACTER_MINE_H
#define ORNITHOPTERCHARACTER_MINE_H
#include "AnimatedEntity.h"
#include "RenderableGameObject.h"
#include "SoundManager.h"
#include "WorldTimeManager.h"

/**
 * \brief Represents one of these: https://dune.fandom.com/wiki/Ornithopter
 */
class OrnithopterCharacter : public AnimatedEntity
{
public:

	OrnithopterCharacter(const WorldTimeManager* time, SoundManager* sound, RenderableGameObject* ornithropterObject, AnimationSet* animations);

	void onNewFrame() override;
	void draw(Shader& shader) override;

private:
	const WorldTimeManager* _time;

	SoundManager* _sound;

	RenderableGameObject* _model;
	Animator _animator;

	irrklang::ISound* _currentSound = nullptr;

	glm::vec3 _currentPos;
};

#endif