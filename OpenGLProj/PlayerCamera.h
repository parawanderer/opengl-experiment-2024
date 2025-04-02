#ifndef PLAYERCAMERA_MINE_H
#define PLAYERCAMERA_MINE_H

#include "Terrain.h"
#include "Camera.h"

struct PlayerMovementKeyStates {
	bool W = false;
	bool A = false;
	bool S = false;
	bool D = false;
};


class PlayerCamera : Camera
{
public:
	PlayerCamera(glm::vec3 initialPos, glm::vec3 initialFront, int initialWidth, int initialHeight, float speedMultiplier, float playerHeightWorldSpace);

	void setTerrain(Terrain* terrain);

	glm::vec3 getPos() const override;
	float getFov() const override;

	glm::mat4 getView() const override;
	void onNewFrame() override;
	void processInput(GLFWwindow* window) override;
	void processScroll(GLFWwindow* window, double xoffset, double yoffset) override;
	void processMouse(GLFWwindow* window, double xpos, double ypos) override;
	void processKey(GLFWwindow* window, int key, int scancode, int action, int mods);

	void teleportToFloor();
private:
	Terrain* _terrain = nullptr;
	float _playerHeight;

	PlayerMovementKeyStates _keyStates;

	bool _isAnyMovementKeyPressed() const;
	void _assignWASDValue(bool state, int key);
};

#endif