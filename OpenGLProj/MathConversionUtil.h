#ifndef MATHCONVERSIONUTIL_MINE_H
#define MATHCONVERSIONUTIL_MINE_H

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/vector2.h>
#include <assimp/vector3.h>
#include <glm/fwd.hpp>

namespace MathConversionUtil
{
	glm::vec3 convert(const aiVector3D& vec3);

	glm::vec2 convert(const aiVector2D& vec2);

	glm::mat4 convert(const aiMatrix4x4& mat4);

	glm::quat convert(const aiQuaternion& orientation);
}

#endif
