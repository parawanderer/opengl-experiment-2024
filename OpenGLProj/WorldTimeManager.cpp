#include "WorldTimeManager.h"

#include <GLFW/glfw3.h>

WorldTimeManager::WorldTimeManager()
{}

void WorldTimeManager::onNewFrame()
{
	float currentFrame = glfwGetTime();
	this->_deltaTime = currentFrame - this->_lastFrame;
	this->_lastFrame = currentFrame;
}

float WorldTimeManager::getDeltaTime() const
{
	return this->_deltaTime;
}

float WorldTimeManager::getCurrentTime() const
{
	return this->_lastFrame;
}
