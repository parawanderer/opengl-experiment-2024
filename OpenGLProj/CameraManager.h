#ifndef CAMERAMANAGER_MINE_H
#define CAMERAMANAGER_MINE_H

#include "PlayerCamera.h"
#include "WorldTimeManager.h"

class CameraManager
{
public:
	CameraManager(
		WorldTimeManager* time,
		bool startWithPlayer,
		glm::vec3 initialPos,
		glm::vec3 initialFront,
		int initialWidth,
		int initialHeight,
		float speedMultiplier);

	void setTerrain(Terrain* terrain);

	void switchToPlayer();
	void switchToNoClip();

	void beforeLoop();
	void processInput(GLFWwindow* window);

	Camera* getCurrentCamera() const;

	PlayerCamera* getPlayerCamera();

	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void processKey(GLFWwindow* window, int key, int scancode, int action, int mods);

	glm::vec3 getPos() const;

	bool isPlayerCamera() const;
private:
	Camera _noclipCam;
	PlayerCamera _playerCam;

	Camera* _currentCam;
};

#endif