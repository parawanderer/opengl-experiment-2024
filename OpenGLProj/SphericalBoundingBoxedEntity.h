#pragma once
#include <glm/vec3.hpp>

class SphericalBoundingBoxedEntity
{
public:
	virtual ~SphericalBoundingBoxedEntity() = default;
	virtual float getRadiusSphericalBoundingBox() = 0;
	virtual glm::vec3 getBoundMidPoint() = 0;
};
