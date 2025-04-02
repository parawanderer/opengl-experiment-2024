#ifndef ANIMATOR_MINE_H
#define ANIMATOR_MINE_H
#include "Animation.h"
#include "AnimationManager.h"

/**
 * \brief Main reference: https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
 */
class Animator
{
public:
	Animator(AnimationManager* animation);

	/**
	 * \brief advances current time with a rate of the ticksPerSecond of the animation and computes required bone transforms
	 */
	void updateAnimation(float deltaTime);

	//void playAnimation(Animation* animation);
	void playAnimation(const std::string& animationName);

	/**
	 * \brief Recursively computes bone transformations for the animation of the model, parent mesh nodes to children
	 * (if parent transforms then children must inherit parent's transform)
	 */
	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	const std::vector<glm::mat4>& getFinalBoneMatrices();

private:
	std::vector<glm::mat4> _finalBoneMatrices;
	AnimationManager* _animationManager;
	Animation* _currentAnimation;
	float _currentTime;
	float _deltaTime;
};


#endif