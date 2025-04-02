#include "PlayerCamera.h"

#include <set>
#include <glm/ext/matrix_transform.hpp>

#define PI 3.14159265359

std::set _MOVEMENT_KEYS = {
	GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D
};


PlayerCamera::PlayerCamera(
	glm::vec3 initialPos, 
	glm::vec3 initialFront, 
	int initialWidth, 
	int initialHeight,
	float speedMultiplier, 
	float playerHeightWorldSpace)
: Camera(initialPos, initialFront, initialWidth, initialHeight, speedMultiplier), _playerHeight(playerHeightWorldSpace)
{
}

void PlayerCamera::setTerrain(Terrain* terrain)
{
	this->_terrain = terrain;
}

glm::mat4 PlayerCamera::getView() const
{
	glm::mat4 view = Camera::getView();
	if (this->_isAnyMovementKeyPressed())
	{
		// transform to get something that looks like "movement" (headbobbing)
		const double t = glfwGetTime();
		view = glm::translate(view, glm::vec3(
			sin(t * 5.0f * PI) / 32.0f, // x
			cos(t * 5.0f * PI) / 8.0f, // y
			0.0f // z
		));
	}
	return view;
}

void PlayerCamera::onNewFrame()
{
	Camera::onNewFrame();
}

void PlayerCamera::processInput(GLFWwindow* window)
{
	const float cameraSpeed = _deltaTime * this->_speedMultiplier;
	bool hasMoved = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		this->_cameraPos += cameraSpeed * this->_cameraFront;
		hasMoved = true;
	}
		

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		this->_cameraPos -= cameraSpeed * this->_cameraFront;
		hasMoved = true;
	}
		

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		this->_cameraPos -= glm::normalize(glm::cross(this->_cameraFront, this->_cameraUp)) * cameraSpeed;
		hasMoved = true;
	}
		

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		this->_cameraPos += glm::normalize(glm::cross(this->_cameraFront, this->_cameraUp)) * cameraSpeed;
		hasMoved = true;
	}

	if (hasMoved)
	{
		// ignore the 'y' we determined above with this "Player" camera,
		// we have our own "y" that we will get (potentially interpolate) from the height map, using the terrain
		this->_cameraPos.y = this->_terrain->getWorldHeightAt(this->_cameraPos.x, this->_cameraPos.z) + this->_playerHeight;
	}
}

void PlayerCamera::processScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera::processScroll(window, xoffset, yoffset);
}

void PlayerCamera::processMouse(GLFWwindow* window, double xpos, double ypos)
{
	Camera::processMouse(window, xpos, ypos);
}

void PlayerCamera::processKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// https://www.glfw.org/docs/latest/input_guide.html#input_key
	if (action == GLFW_PRESS)
	{
		this->_assignWASDValue(true, key);
	}
	else if (action == GLFW_RELEASE)
	{
		this->_assignWASDValue(false, key);
	}
}

void PlayerCamera::teleportToFloor()
{
	auto ourPos = this->getPos();
	auto worldHeight = this->_terrain->getWorldHeightAt(ourPos.x, ourPos.z);
	if (fabs((ourPos.y - this->_playerHeight) - worldHeight) > 0.01)
	{
		this->_cameraPos.y = worldHeight + this->_playerHeight;
	}
}

bool PlayerCamera::_isAnyMovementKeyPressed() const
{
	return this->_keyStates.W || this->_keyStates.A || this->_keyStates.S || this->_keyStates.D;
}

void PlayerCamera::_assignWASDValue(bool state, int key)
{
	if (key == GLFW_KEY_W) this->_keyStates.W = state;
	if (key == GLFW_KEY_A) this->_keyStates.A = state;
	if (key == GLFW_KEY_S) this->_keyStates.S = state;
	if (key == GLFW_KEY_D) this->_keyStates.D = state;
}

glm::vec3 PlayerCamera::getPos() const
{
	return Camera::getPos();
}

float PlayerCamera::getFov() const
{
	return Camera::getFov();
}
