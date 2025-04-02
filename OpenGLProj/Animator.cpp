#include "Animator.h"

#include "AnimationSet.h"

#define MAX_BONES 100

Animator::Animator(AnimationSet* animation)
:
_animationManager(animation),
_currentAnimation(nullptr),
_currentTime(0.0f)
{
	this->_finalBoneMatrices.reserve(MAX_BONES);

	for (int i = 0; i < MAX_BONES; ++i)
		this->_finalBoneMatrices.emplace_back(1.0f);
}

void Animator::updateAnimation(float deltaTime)
{
	this->_deltaTime = deltaTime;
	if (this->_currentAnimation)
	{
		this->_currentTime += this->_currentAnimation->getTicksPerSecond() * deltaTime;
		this->_currentTime = fmod(this->_currentTime, this->_currentAnimation->getDuration());
		this->calculateBoneTransform(&this->_animationManager->getRootNode(), glm::mat4(1.0f)); // compute transforms from root node
	}
}

// void Animator::playAnimation(Animation* animation)
// {
// 	this->_currentAnimation = animation;
// 	this->_currentTime = 0.0f;
// }

void Animator::playAnimation(const std::string& animationName)
{
	Animation* animation = this->_animationManager->getAnimation(animationName);
	this->_currentAnimation = animation;
	this->_currentTime = 0.0f;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	const std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* bone = this->_currentAnimation->findBone(nodeName);

	if (bone)
	{
		bone->update(this->_currentTime); // interpolates all the bones and stores the local transform
		nodeTransform = bone->getLocalTransform(); // retrieve that local transform ^
	}

	const glm::mat4 globalTransformation = parentTransform * nodeTransform;

	std::map<std::string, BoneInfo>& boneInfoMap = this->_currentAnimation->getBoneIdMap();
	if (boneInfoMap.contains(nodeName))
	{
		const int index = boneInfoMap[nodeName].id;
		const glm::mat4 offset = boneInfoMap[nodeName].offset;
		this->_finalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; ++i)
	{
		this->calculateBoneTransform(&node->children[i], globalTransformation);
	}
}

const std::vector<glm::mat4>& Animator::getFinalBoneMatrices()
{
	return this->_finalBoneMatrices;
}
