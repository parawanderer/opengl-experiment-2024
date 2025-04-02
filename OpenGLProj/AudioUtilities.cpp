#include "AudioUtilities.h"

irrklang::vec3df AudioUtilities::convert(const glm::vec3& position)
{
	return irrklang::vec3df(position.x, position.y, -position.z);
}
