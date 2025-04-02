#ifndef DRAWABLEENTITY_MINE_H
#define DRAWABLEENTITY_MINE_H
#include "Shader.h"

/**
 * \brief An entity that can be rendered
 */
class DrawableEntity
{
public:
	virtual ~DrawableEntity() = default;
	virtual void draw(Shader& shader) = 0;
};

#endif