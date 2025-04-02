#ifndef SPHERICALBOUNDINGBOXEDENTITY_MINE_H
#define SPHERICALBOUNDINGBOXEDENTITY_MINE_H

#include <glm/vec3.hpp>

/**
 * \brief An entity that has a spherical bounding box (used for intersection computation)
 */
class SphericalBoundingBoxedEntity
{
public:
	virtual ~SphericalBoundingBoxedEntity() = default;
	virtual float getRadiusSphericalBoundingBox() = 0;
	virtual glm::vec3 getBoundMidPoint() = 0;
};

#endif