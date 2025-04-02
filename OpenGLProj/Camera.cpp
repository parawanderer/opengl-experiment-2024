#include "Camera.h"

#include <glm/ext/matrix_transform.hpp>

Camera::Camera(glm::vec3 initialPos, glm::vec3 initialFront, int initialWidth, int initialHeight, float speedMultiplier)
: _cameraPos(initialPos), _cameraFront(initialFront), _lastX(initialWidth / 2), _lastY(initialHeight / 2), _speedMultiplier(speedMultiplier)
{}

glm::mat4 Camera::getView() const
{
	return glm::lookAt(this->_cameraPos, this->_cameraPos + this->_cameraFront, this->_cameraUp);
}

glm::vec3 Camera::getPos() const
{
	return this->_cameraPos;
}

float Camera::getFov() const
{
	return this->_fov;
}

glm::vec3 Camera::getFront() const
{
	return this->_cameraFront;
}

void Camera::onNewFrame()
{
	float currentFrame = glfwGetTime();
	this->_deltaTime = currentFrame - this->_lastFrame;
	this->_lastFrame = currentFrame;
}

void Camera::processInput(GLFWwindow* window)
{
	const float cameraSpeed = _deltaTime * this->_speedMultiplier;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		this->_cameraPos += cameraSpeed * this->_cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		this->_cameraPos -= cameraSpeed * this->_cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		this->_cameraPos -= glm::normalize(glm::cross(this->_cameraFront, this->_cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		this->_cameraPos += glm::normalize(glm::cross(this->_cameraFront, this->_cameraUp)) * cameraSpeed;
}

void Camera::processScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	this->_fov -= (float)yoffset;
	if (this->_fov < 1.0f) this->_fov = 1.0f;
	if (this->_fov > 45.0f) this->_fov = 45.0f;
}

void Camera::processMouse(GLFWwindow* window, double xpos, double ypos)
{
	if (this->_firstMouse)
	{
		this->_lastX = xpos;
		this->_lastY = ypos;
		this->_firstMouse = false;
	}

	float xoffset = xpos - this->_lastX;
	float yoffset = this->_lastY - ypos; // reversed as y-coordinates range from bottom to top
	this->_lastX = xpos;
	this->_lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	this->_yaw += xoffset;
	this->_pitch += yoffset;

	if (this->_pitch > 89.0f) this->_pitch = 89.0f;
	if (this->_pitch < -89.0f) this->_pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));
	direction.y = sin(glm::radians(this->_pitch));
	direction.z = sin(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));
	this->_cameraFront = glm::normalize(direction);
}

void Camera::processKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// nothing
}
