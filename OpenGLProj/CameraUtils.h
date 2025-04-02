#ifndef CAMERAUTILS_MINE_H
#define CAMERAUTILS_MINE_H

#include <glm/fwd.hpp>

namespace CameraUtils
{
	glm::mat4 getCarriedItemModelTransform(const glm::mat4& view, const float t, bool isMoving, bool isSpeeding);
}

#endif