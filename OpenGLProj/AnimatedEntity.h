#ifndef ANIMATEDENTITY_MINE_H
#define ANIMATEDENTITY_MINE_H
#include "Animator.h"
#include "DrawableEntity.h"
#include "FrameRequester.h"

/**
 * \brief An entity that defines per-frame animation
 */
class AnimatedEntity : public FrameRequester, public DrawableEntity
{
protected:
	/**
	 * \brief Generic reusable means of setting up animations in a given frame.
	 * \param shader			Shader to use for animation rendering
	 * \param transforms		Transforms being applied to the model
	 */
	void setupEntityShaderForAnim(Shader& shader, const std::vector<glm::mat4>& transforms);
	void clearEntityShaderForAnim(Shader& shader);
};

#endif