#include "Animator.h"

#include <iostream>

#include "AnimationSet.h"

#define MAX_BONES 100

// TODO: rewrite the interpolated dual animation stuff because it is confusing and I did it last minute to get it into the opdracht limits

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

void Animator::updateAnimation2(float deltaTime, float interpolationFactor)
{
	this->_deltaTime = deltaTime;
	if (this->_currentAnimation && this->_currentAnimation2)
	{
		this->_currentTime += this->_currentAnimation->getTicksPerSecond() * deltaTime;
		this->_currentTime = fmod(this->_currentTime, this->_currentAnimation->getDuration());

		this->_currentTime2 += this->_currentAnimation2->getTicksPerSecond() * deltaTime;
		this->_currentTime2 = fmod(this->_currentTime2, this->_currentAnimation2->getDuration());

		this->calculateBoneTransform2(&this->_animationManager->getRootNode(), glm::mat4(1.0f), interpolationFactor); // compute transforms from root node
	}
}

void Animator::flipAnimation1And2()
{
	auto tmp = this->_currentAnimation;
	auto tmp2 = this->_currentTime;
	this->_currentAnimation = this->_currentAnimation2;
	this->_currentTime = this->_currentTime2;

	this->_currentAnimation2 = tmp;
	this->_currentTime2 = tmp2;
}

void Animator::clearAnimation2()
{
	this->_currentAnimation2 = nullptr;
	this->_currentTime2 = 0.0f;
}

Animation* Animator::getAnimationByName(const std::string& animationName) const
{
	return this->_animationManager->getAnimation(animationName);
}

void Animator::playAnimation(Animation* animation)
{
	this->_currentAnimation = animation;
	this->_currentTime = 0.0f;
	this->_currentAnimation2 = nullptr;
	this->_currentTime2 = 0.0f;
}

void Animator::playAnimation(const std::string& animationName)
{
	Animation* animation = this->_animationManager->getAnimation(animationName);
	this->_currentAnimation = animation;
	this->_currentTime = 0.0f;
	this->_currentAnimation2 = nullptr;
	this->_currentTime2 = 0.0f;
}

void Animator::playAnimations(Animation* animation1, Animation* animation2)
{
	this->_currentAnimation = animation1;
	this->_currentTime = 0.0f;
	this->_currentAnimation2 = animation2;
	this->_currentTime2 = 0.0f;
}

void Animator::startPlaying2ndAnimation(Animation* animation2)
{
	this->_currentAnimation2 = animation2;
	this->_currentTime2 = 0.0f;
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

void Animator::calculateBoneTransform2(const AssimpNodeData* node, glm::mat4 parentTransform, float interpolationFactorAnimation1)
{
	if (this->_currentAnimation2 == nullptr) throw std::exception("No second animation specified");
	if (interpolationFactorAnimation1 < 0.0 || interpolationFactorAnimation1 > 1.0) throw std::exception("Invalid interpolation factor provided");

	this->calculateBoneTransform2Inner(node, parentTransform, interpolationFactorAnimation1);
}

const std::vector<glm::mat4>& Animator::getFinalBoneMatrices()
{
	return this->_finalBoneMatrices;
}

void Animator::calculateBoneTransform2Inner(const AssimpNodeData* node, glm::mat4 parentTransform,
	float interpolationFactorAnimation1)
{
	const std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* bone1 = this->_currentAnimation->findBone(nodeName);
	Bone* bone2 = this->_currentAnimation2->findBone(nodeName);

	if (bone1) bone1->update(this->_currentTime);
	if (bone2) bone2->update(this->_currentTime2);

	if (bone1 && bone2)
	{
		const InterpolatedTransform& transformData1 = bone1->getLocalInterpolatedTransforms();
		const InterpolatedTransform& transformData2 = bone2->getLocalInterpolatedTransforms();
		nodeTransform = Bone::interpolateBetweenTwo(transformData1, transformData2, interpolationFactorAnimation1);
	}
	else if (bone1) // only bone 1
	{
		nodeTransform = bone1->getLocalTransform();
	}
	else if (bone2) // only bone 2
	{
		nodeTransform = bone2->getLocalTransform();
	}

	const glm::mat4 globalTransformation = parentTransform * nodeTransform;

	std::map<std::string, BoneInfo>& boneInfoMap = this->_currentAnimation->getBoneIdMap(); // notably these maps should be the same between both animations
	if (!this->_currentAnimation2->getBoneIdMap().contains(nodeName) && boneInfoMap.contains(nodeName)) throw std::exception("Bone node mismatch in second animation");

	if (boneInfoMap.contains(nodeName))
	{
		const int index = boneInfoMap[nodeName].id;
		const glm::mat4 offset = boneInfoMap[nodeName].offset;
		this->_finalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; ++i)
	{
		this->calculateBoneTransform2Inner(&node->children[i], globalTransformation, interpolationFactorAnimation1);
	}
}
