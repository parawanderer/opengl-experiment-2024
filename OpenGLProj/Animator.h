#ifndef ANIMATOR_MINE_H
#define ANIMATOR_MINE_H
#include "Animation.h"
#include "AnimationSet.h"

/**
 * \brief Controller for playing animations and moving the animation state forward in time.
 *
 * Main reference: https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
 */
class Animator
{
public:
	Animator(AnimationSet* animation);

	/**
	 * \brief advances current time with a rate of the ticksPerSecond of the animation and computes required bone transforms
	 */
	void updateAnimation(float deltaTime);

	/**
	 * \brief Play the animation by name
	 */
	void playAnimation(const std::string& animationName);

	/**
	 * \brief Recursively computes bone transformations for the animation of the model, parent mesh nodes to children
	 * (if parent transforms then children must inherit parent's transform)
	 */
	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	/**
	 * \brief Gets the bone matrices used for animation. Must be called after calling updateAnimation();
	 * \return Bone matrices to use when rendering the model to which this Animator belongs.
	 */
	const std::vector<glm::mat4>& getFinalBoneMatrices();

private:
	std::vector<glm::mat4> _finalBoneMatrices;
	AnimationSet* _animationManager;
	Animation* _currentAnimation;
	float _currentTime;
	float _deltaTime;
};


#endif