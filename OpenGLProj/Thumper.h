#ifndef THUMPER_MINE_H
#define THUMPER_MINE_H
#include "AnimatedEntity.h"
#include "AnimationManager.h"
#include "Animator.h"
#include "SphericalBoundingBoxedEntity.h"
#include "SphericalBoxedGameObject.h"
#include "WorldTimeManager.h"

class Thumper : public AnimatedEntity, public SphericalBoundingBoxedEntity
{
public:

	enum class STATE
	{
		DISABLED = 0,
		ACTIVATED = 1
	};


	Thumper(WorldTimeManager* time, SphericalBoxedGameObject* thumper, AnimationManager* animations);

	void onNewFrame() override;
	void draw(Shader& shader) override;

	void setState(STATE newState);
	STATE getState() const;

	SphericalBoxedGameObject* getObjectModel() const;

	float getRadiusSphericalBoundingBox() override;
	glm::vec3 getBoundMidPoint() override;

	void setModelTransform(const glm::mat4& model);

private:
	WorldTimeManager* _time;
	SphericalBoxedGameObject* _model;
	Animator _animator;

	STATE _state = STATE::DISABLED;
};

#endif