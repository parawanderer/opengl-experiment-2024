#ifndef THUMPER_MINE_H
#define THUMPER_MINE_H
#include "AnimatedEntity.h"
#include "AnimationSet.h"
#include "Animator.h"
#include "SoundManager.h"
#include "SphericalBoundingBoxedEntity.h"
#include "SphericalBoxedGameObject.h"
#include "WorldTimeManager.h"

/**
 * \brief Game entity for a Thumper. Manages the actual game state of an instance of a specific Thumper in the game.
 *
 *	Represents one of these: https://dune.fandom.com/wiki/Thumper
 */
class Thumper : public AnimatedEntity, public SphericalBoundingBoxedEntity
{
public:

	enum class STATE
	{
		DISABLED = 0,
		ACTIVATED = 1
	};


	Thumper(const WorldTimeManager* time, SoundManager* sound, SphericalBoxedGameObject* thumper, AnimationSet* animations);

	void onNewFrame() override;
	void draw(Shader& shader) override;

	void setState(STATE newState);
	STATE getState() const;
	void setIsCarried(bool isCarried);

	SphericalBoxedGameObject* getObjectModel() const;

	float getRadiusSphericalBoundingBox() override;
	glm::vec3 getBoundMidPoint() override;

	void setPosition(const glm::vec3& newPosition);

	void drawCarried(Shader& shader, const glm::mat4& view, const float t, bool isMoving, bool isSpeeding);

private:
	const WorldTimeManager* _time;
	SoundManager* _sound;

	SphericalBoxedGameObject* _model;
	Animator _animator;

	AudioPlayer _currentSound;

	glm::vec3 _currentPos = glm::vec3(0.0, 0.0, 0.0);
	bool _isCarried = false;
	STATE _state = STATE::DISABLED;

	bool _isActivating = false;
	float _activatingSwitchToActiveAt = 0.0f;

	void handleActivation();
	void handleDeactivation();

	void updateModelTransform();
};

#endif