#include "MathConversionUtil.h"

#include <glm/detail/type_quat.hpp>

glm::vec3 MathConversionUtil::convert(const aiVector3D& vec3)
{
	return glm::vec3(vec3.x, vec3.y, vec3.z);
}

glm::vec2 MathConversionUtil::convert(const aiVector2D& vec2)
{
	return glm::vec2(vec2.x, vec2.y);
}

glm::mat4 MathConversionUtil::convert(const aiMatrix4x4& from)
{
	// return glm::mat4(
	// 	mat4.a1, mat4.b1, mat4.c1, mat4.d1, // first column
	// 	mat4.a2, mat4.b2, mat4.c2, mat4.d2, // second column
	// 	mat4.a3, mat4.b3, mat4.c3, mat4.d3, // third colum
	// 	mat4.a4, mat4.b4, mat4.c4, mat4.d4 // fourth column
	// );

	glm::mat4 to;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

glm::quat MathConversionUtil::convert(const aiQuaternion& orientation)
{
	return glm::quat(orientation.w, orientation.x, orientation.y, orientation.z);
}
