#ifndef CAMERA_MINE_H
#define CAMERA_MINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include "WorldTimeManager.h"

class Camera
{
public:
	virtual ~Camera() = default;
	Camera(WorldTimeManager* time, glm::vec3 initialPos, glm::vec3 initialFront, int initialWidth, int initialHeight, float speedMultiplier);

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

	/**
	 * \brief cameraFront or cameraDirection. This is a normalized vector!
	 */
	glm::vec3 getFront() const;

	virtual void processInput(GLFWwindow* window);
	virtual void processScroll(GLFWwindow* window, double xoffset, double yoffset);
	virtual void processMouse(GLFWwindow* window, double xpos, double ypos);
	virtual void processKey(GLFWwindow* window, int key, int scancode, int action, int mods);

	virtual bool isMoving() const;
	virtual bool isSpeeding() const;

protected:
	WorldTimeManager* _time;

	glm::vec3 _cameraPos;
	glm::vec3 _cameraFront;
	glm::vec3 _cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float _pitch = 0.0f;
	float _yaw = -90.0f;

	float _lastX;
	float _lastY;

	bool _firstMouse = true;

	float _fov = 45.0f;

	float _speedMultiplierBase;

	float getDeltaTime() const;
};

#endif