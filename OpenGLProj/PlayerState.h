#ifndef PLAYER_MINE_H
#define PLAYER_MINE_H

#include "CarriedGameObject.h"
#include "PlayerCamera.h"
#include "PlayerCameraEventSubscriber.h"

/**
 * \brief Represents the player's game state
 */
class PlayerState : public PlayerCameraEventSubscriber
{
public:
	PlayerState(SoundManager* sound, const PlayerCamera* playerCamera);

	bool hasCarriedItem();

	void setCarriedItem(const CarriedGameObject& item);
	CarriedGameObject& getCarriedItem();

	CarriedGameObject removeCarriedItem();

	void onNewPos(const glm::vec3& newPos) override;
	void onStartWalking() override;
	void onStopMoving() override;
	void onStartJumping() override;
	void onStopJumping() override;
	void onStartRunning() override;

private:
	SoundManager* _sound;
	const PlayerCamera* _playerCamera;
	char _movementState = 0;
	bool _isInAirDueToJump = false;

	AudioPlayer _currentWalkSound;
	AudioPlayer _currentJumpSound;

	bool _hasItem = false;
	CarriedGameObject _carriedItem;

	glm::vec3 getUnderFeetPos(const glm::vec3& playerPos) const;
};


#endif