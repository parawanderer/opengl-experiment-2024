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

	void updateAnimation2(float deltaTime, float interpolationFactor);

	void flipAnimation1And2();

	void clearAnimation2();

	/**
	 * \brief Get animation details by animation name
	 */
	Animation* getAnimationByName(const std::string& animationName) const;

	/**
	 * \brief play a specific animation. Animation should have been retreived using getAnimationByName();
	 */
	void playAnimation(Animation* animation);

	/**
	 * \brief Play the animation by name
	 */
	void playAnimation(const std::string& animationName);


	void playAnimations(Animation* animation1, Animation* animation2);

	void startPlaying2ndAnimation(Animation* animation2);

	/**
	 * \brief Recursively computes bone transformations for the animation of the model, parent mesh nodes to children
	 * (if parent transforms then children must inherit parent's transform)
	 *
	 * Single-animation.
	 */
	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	/**
	 * \brief Equivalent to calculateBoneTransform() but allows interpolating between two animations. Prerequisite is calling playAnimations(animation1, animation2)
	 * to set up the animations to interpolate between.
	 *
	 * \param interpolationFactorAnimation1		interpolation factor for first animation. Must be in range [0, 1]. second animation will be weighted with (1 - interpolationFactorAnimation1).
	 */
	void calculateBoneTransform2(const AssimpNodeData* node, glm::mat4 parentTransform, float interpolationFactorAnimation1);

	/**
	 * \brief Gets the bone matrices used for animation. Must be called after calling updateAnimation();
	 * \return Bone matrices to use when rendering the model to which this Animator belongs.
	 */
	const std::vector<glm::mat4>& getFinalBoneMatrices();

private:
	std::vector<glm::mat4> _finalBoneMatrices;
	AnimationSet* _animationManager;
	Animation* _currentAnimation;
	Animation* _currentAnimation2 = nullptr;
	float _currentTime;
	float _currentTime2;
	float _deltaTime;

	void calculateBoneTransform2Inner(const AssimpNodeData* node, glm::mat4 parentTransform, float interpolationFactorAnimation1);
};


#endif