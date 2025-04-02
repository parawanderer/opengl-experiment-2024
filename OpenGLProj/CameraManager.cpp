#include "CameraManager.h"

constexpr auto PLAYER_HEIGHT_WOLD_SPACE = 2.0f;
constexpr auto STARTING_VELOCITY_OF_JUMP = 5.0f;

CameraManager::CameraManager(
	bool startWithPlayer,
	glm::vec3 initialPos,
	glm::vec3 initialFront,
	int initialWidth,
	int initialHeight,
	float speedMultiplier) :
_noclipCam(Camera(initialPos, initialFront, initialWidth, initialHeight, speedMultiplier)),
_playerCam(PlayerCamera(initialPos, initialFront, initialWidth, initialHeight, speedMultiplier, PLAYER_HEIGHT_WOLD_SPACE, STARTING_VELOCITY_OF_JUMP)),
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
	this->_currentCam->processInput(window);
}

Camera* CameraManager::getCurrentCamera() const
{
	return this->_currentCam;
}

void CameraManager::onNewFrame()
{
	this->_currentCam->onNewFrame();
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

bool CameraManager::isPlayer() const
{
	return this->_currentCam == &this->_playerCam;
}
