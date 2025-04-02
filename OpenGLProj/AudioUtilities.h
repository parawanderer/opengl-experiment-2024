#ifndef AUDIOUTILITIES_MINE_H
#define AUDIOUTILITIES_MINE_H
#include <glm/vec3.hpp>
#include <irrKlang/ik_vec3d.h>

namespace AudioUtilities
{
	irrklang::vec3df convert(const glm::vec3& position);
}

#endif