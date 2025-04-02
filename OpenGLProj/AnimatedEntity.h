#ifndef ANIMATEDENTITY_MINE_H
#define ANIMATEDENTITY_MINE_H
#include "Animator.h"

/**
 * \brief An entity that defines per-frame animation
 */
class AnimatedEntity
{
public:
	virtual ~AnimatedEntity() = default;
	virtual void onNewFrame() = 0;

	virtual void draw(Shader& shader) = 0;

protected:
	void setupEntityShaderForAnim(Shader& shader, const std::vector<glm::mat4>& transforms);
	void clearEntityShaderForAnim(Shader& shader);
};

#endif