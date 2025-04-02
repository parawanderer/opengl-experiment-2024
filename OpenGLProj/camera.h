#ifndef CAMERA_MINE_H
#define CAMERA_MINE_H

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

class Camera
{
public:
	Camera(glm::vec3 initialPos, glm::vec3 initialFront, int initialWidth, int initialHeight, float speedMultiplier);

	/**
	 * \return The view transformation matrix
	 */
	virtual glm::mat4 getView() const;
	/**
	 * \return The position of the camera
	 */
	virtual glm::vec3 getPos() const;
	/**
	 * \return the FOV of the camera in degrees
	 */
	virtual float getFov() const;

	virtual void onNewFrame();
	virtual void processInput(GLFWwindow* window);
	virtual void processScroll(GLFWwindow* window, double xoffset, double yoffset);
	virtual void processMouse(GLFWwindow* window, double xpos, double ypos);

protected:
	glm::vec3 _cameraPos;
	glm::vec3 _cameraFront;
	glm::vec3 _cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	float _deltaTime = 0.0f; // time between current and last frame
	float _lastFrame = 0.0f; // time of last frame

	float _pitch = 0.0f;
	float _yaw = -90.0f;

	float _lastX;
	float _lastY;

	bool _firstMouse = true;

	float _fov = 45.0f;

	float _speedMultiplier;
};

#endif