#ifndef PLAYERCAMERA_MINE_H
#define PLAYERCAMERA_MINE_H

#include "Terrain.h"
#include "Camera.h"
#include "PlayerCameraEventSubscriber.h"

struct PlayerMovementKeyStates {
	bool W = false;
	bool A = false;
	bool S = false;
	bool D = false;
};


class PlayerCamera : public Camera
{
public:
	PlayerCamera(WorldTimeManager* time, glm::vec3 initialPos, glm::vec3 initialFront, int initialWidth, int initialHeight, float speedMultiplier, float playerHeightWorldSpace, float startingVelocityOfJump);

	void setTerrain(Terrain* terrain);

	glm::vec3 getPos() const override;
	glm::vec3 getPosIncludingJump() const;
	float getFov() const override;

	float getPlayerHeight() const;

	glm::mat4 getView() const override;
	void processInput(GLFWwindow* window) override;
	void processScroll(GLFWwindow* window, double xoffset, double yoffset) override;
	void processMouse(GLFWwindow* window, double xpos, double ypos) override;
	void processKey(GLFWwindow* window, int key, int scancode, int action, int mods) override;

	bool isMoving() const override;
	bool isSpeeding() const override;

	void teleportToFloor();

	void addSubscriber(PlayerCameraEventSubscriber* subscriber);

private:
	Terrain* _terrain = nullptr;
	float _playerHeight;
	float _jumpStartVelocity; // velocity (y dimension only)  in (m/s)

	PlayerMovementKeyStates _keyStates;
	bool _lastShiftPressed = false;
	bool _isShiftPressed = false;
	bool _isJumping = false;
	bool _isMoving = false;

	float _currentYVelocity = 0.0f;
	float _yDisplacement = 0.0f;

	std::vector<PlayerCameraEventSubscriber*> _subscribers;

	bool isAnyMovementKeyPressed() const;
	void assignWASDValue(bool state, int key);
};

#endif