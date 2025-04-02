#include "PlayerCamera.h"

#include <glm/ext/matrix_transform.hpp>

#define _USE_MATH_DEFINES
#include "math.h"

// (Acceleration due to gravity) m/s^2
#define G -9.81f

PlayerCamera::PlayerCamera(
	WorldTimeManager* time,
	glm::vec3 initialPos, 
	glm::vec3 initialFront, 
	int initialWidth, 
	int initialHeight,
	float speedMultiplier, 
	float playerHeightWorldSpace,
	float startingVelocityOfJump)
: Camera(time, initialPos, initialFront, initialWidth, initialHeight, speedMultiplier),
_playerHeight(playerHeightWorldSpace),
_jumpStartVelocity(startingVelocityOfJump)
{
}

void PlayerCamera::setTerrain(Terrain* terrain)
{
	this->_terrain = terrain;
}

glm::mat4 PlayerCamera::getView() const
{
	//glm::mat4 view = glm::lookAt(this->_cameraPos, this->_cameraPos + this->_cameraFront, this->_cameraUp);
	glm::vec3 playerPos = this->getPosIncludingJump();
	glm::mat4 view = glm::lookAt(playerPos, playerPos + this->_cameraFront, this->_cameraUp);

	if (!this->_isJumping && this->isAnyMovementKeyPressed())
	{
		const float sprintingMultiplier = this->_isShiftPressed ? 1.5f : 1.0f;
		// transform to get something that looks like "movement" (head-bobbing)
		const double t = glfwGetTime();
		view = glm::translate(view, glm::vec3(
			0, //sin(t * 5.0f * M_PI * sprintingMultiplier) / 40.0, // x
			cos(t * 5.0f * M_PI * sprintingMultiplier) / 32.0, // y
			0.0f // z
		));
	}
	return view;
}

void PlayerCamera::processInput(GLFWwindow* window)
{
	if (this->_isJumping) // update jumping state
	{
		// kinematics https://en.wikipedia.org/wiki/Equations_of_motion
		this->_yDisplacement = this->_yDisplacement + (this->_currentYVelocity * this->getDeltaTime()) + (0.5 * G * (this->getDeltaTime() * this->getDeltaTime())); // update y pos
		this->_currentYVelocity = (G * this->getDeltaTime()) + this->_currentYVelocity; // update remaining velocity

		if (this->_yDisplacement <= 0.0f) // reached limit of displacement
		{
			this->_yDisplacement = 0.0f;
			this->_currentYVelocity = 0;
			this->_isJumping = false;
		}
	}

	const float sprintingMultiplier = this->_isShiftPressed ? 2.0f : 1.0f;
	const float jumpingMultiplier = this->_isJumping ? 0.5 : 1.0f; // decrease amount of (x,z) movement we can do while jumping
	const float cameraSpeed = this->getDeltaTime() * (this->_speedMultiplier * sprintingMultiplier * jumpingMultiplier);
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

	if (!this->_isJumping && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		// jump when started jumping
		this->_isJumping = true;
		this->_currentYVelocity = this->_jumpStartVelocity;
		// 1. first we kind of "sit down" (camera moves down for a few frames) -> ~ cos(t) function until pi
		// 2. then we do the traveling with upwards velocity in the y direction. The x & z direction stay as they are for now
		// 3. there'll be a max height where the acceleration due to gravity counteracts the velocity
		// 4. then we fall down (only gravity acts on us)

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
		this->assignWASDValue(true, key);
	}
	else if (action == GLFW_RELEASE)
	{
		this->assignWASDValue(false, key);
	}
}

bool PlayerCamera::isMoving() const
{
	return !this->_isJumping && this->isAnyMovementKeyPressed();
}

bool PlayerCamera::isSpeeding() const
{
	return this->_isShiftPressed && this->isMoving();
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

bool PlayerCamera::isAnyMovementKeyPressed() const
{
	return this->_keyStates.W || this->_keyStates.A || this->_keyStates.S || this->_keyStates.D;
}

void PlayerCamera::assignWASDValue(bool state, int key)
{
	if (key == GLFW_KEY_W) this->_keyStates.W = state;
	if (key == GLFW_KEY_A) this->_keyStates.A = state;
	if (key == GLFW_KEY_S) this->_keyStates.S = state;
	if (key == GLFW_KEY_D) this->_keyStates.D = state;
	if (key == GLFW_KEY_LEFT_SHIFT) this->_isShiftPressed = state;
}

glm::vec3 PlayerCamera::getPos() const
{
	return Camera::getPos();
}

glm::vec3 PlayerCamera::getPosIncludingJump() const
{
	glm::vec3 noJumpPos = Camera::getPos();
	return glm::vec3(noJumpPos.x, noJumpPos.y + this->_yDisplacement, + noJumpPos.z);
}

float PlayerCamera::getFov() const
{
	return Camera::getFov();
}
