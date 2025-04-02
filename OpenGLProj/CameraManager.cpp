#include "CameraManager.h"

constexpr auto PLAYER_HEIGHT_WOLD_SPACE = 1.8f;
constexpr auto STARTING_VELOCITY_OF_JUMP = 5.0f;

CameraManager::CameraManager(
	WorldTimeManager* time,
	bool startWithPlayer,
	glm::vec3 initialPos,
	glm::vec3 initialFront,
	int initialWidth,
	int initialHeight,
	float speedMultiplier) :
_noclipCam(Camera(time, initialPos, initialFront, initialWidth, initialHeight, speedMultiplier * 2.0f)),
_playerCam(PlayerCamera(time, initialPos, initialFront, initialWidth, initialHeight, speedMultiplier / 2.0f, PLAYER_HEIGHT_WOLD_SPACE, STARTING_VELOCITY_OF_JUMP)),
_currentCam(startWithPlayer ? &this->_playerCam : &this->_noclipCam)
{}

void CameraManager::setTerrain(Terrain* terrain)
{
	this->_playerCam.setTerrain(terrain);
}

void CameraManager::switchToPlayer()
{
	this->_currentCam = dynamic_cast<Camera*>(&this->_playerCam);
}

void CameraManager::switchToNoClip()
{
	this->_currentCam = &this->_noclipCam;
}

void CameraManager::beforeLoop()
{
	this->_playerCam.teleportToFloor();
}

void CameraManager::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) // for "observer"
		this->switchToNoClip();

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // for "player"
		this->switchToPlayer();

	this->_currentCam->processInput(window);
}

Camera* CameraManager::getCurrentCamera() const
{
	return this->_currentCam;
}

PlayerCamera* CameraManager::getPlayerCamera()
{
	return &this->_playerCam;
}

void CameraManager::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	this->_currentCam->processMouse(window, xpos, ypos);
}

void CameraManager::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	this->_currentCam->processScroll(window, xoffset, yoffset);
}

void CameraManager::processKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	this->_currentCam->processKey(window, key, scancode, action, mods);
}

glm::vec3 CameraManager::getPos() const
{
	return this->_currentCam == &this->_noclipCam
		? this->_currentCam->getPos()
		: this->_playerCam.getPosIncludingJump();
}

bool CameraManager::isPlayerCamera() const
{
	return this->_currentCam == &this->_playerCam;
}
