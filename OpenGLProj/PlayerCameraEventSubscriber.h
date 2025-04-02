#ifndef PLAYERCAMERAEVENTSUBSCRIBER_MINE_H
#define PLAYERCAMERAEVENTSUBSCRIBER_MINE_H

/**
 * \brief A subscriber to player camera events
 */
class PlayerCameraEventSubscriber
{
public:
	virtual void onNewPos(const glm::vec3& newPos) = 0;

	virtual void onStartWalking() = 0;

	virtual void onStopMoving() = 0;

	virtual void onStartJumping() = 0;

	virtual void onStopJumping() = 0;

	virtual void onStartRunning() = 0;
};

#endif
