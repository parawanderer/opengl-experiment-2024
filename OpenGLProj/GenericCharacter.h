#ifndef GENERICCHARACTER_MINE_H
#define GENERICCHARACTER_MINE_H
#include <glm/vec3.hpp>

#include "AnimatedEntity.h"

class GenericCharacter : public AnimatedEntity
{
public:
	virtual const glm::vec3& getCurrentPosition() const = 0;
};

#endif