#include "CameraUtils.h"

#define _USE_MATH_DEFINES
#include <glm/ext/matrix_transform.hpp>

#include "math.h"

glm::mat4 getCarriedItemModelTransform(const glm::mat4& view, const float t, bool isMoving, bool isSpeeding, const float objectScale)
{
	glm::mat4 model = view;
	model = glm::inverse(model); // cancel view transformation (makes object stick to the camera)
	const float speedMultiplier = isSpeeding ? 1.5f : 1.0f;
	const float xWobble = isMoving ? (sin(t * 2.0f * M_PI * speedMultiplier) / 200.0f) * speedMultiplier : 0.0;
	const float yWobble = isMoving ? sin(t * 4.0f * M_PI * speedMultiplier) / 200.0f : 0.0;
	const float xRotate = isMoving ? (float)glm::radians(sin(t * 2.0f * M_PI * speedMultiplier) * 2.0f * speedMultiplier) : 0.0;
	model = glm::translate(model, glm::vec3(0.5f + xWobble, -0.4f + yWobble, -1.0f)); // move "forwards", "right" and "down"
	model = glm::scale(model, glm::vec3(objectScale)); // scale it down
	model = glm::rotate(model, -xRotate, glm::vec3(0.0f, 0.0f, 1.0f)); // wobble-related rotate
	model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate to make position look better perspective-wise

	return model;
}
