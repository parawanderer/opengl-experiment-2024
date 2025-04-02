#include "AnimationSet.h"

#include <iostream>
#include <assimp/postprocess.h>

#include "MathConversionUtil.h"

AnimationSet::AnimationSet(const std::string& animationPath, Model* model)
{
	Assimp::Importer importer;
	std::cout << "Loading animation file '" << animationPath << "'" << std::endl;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	if (!scene || !scene->mRootNode) throw std::exception("Animation could not be found!");

	const int animationCount = scene->mNumAnimations;

	std::cout << "Found " << animationCount << " animations for file '" << animationPath << "'" << std::endl;

	this->readHierarchyData(this->_rootNode, scene->mRootNode);

	for (int i = 0; i < animationCount; ++i)
	{
		aiAnimation* anim = scene->mAnimations[i];
		this->_animations[anim->mName.C_Str()] = Animation(anim, model);
	}
}

int AnimationSet::getAnimationCount() const
{
	return this->_animations.size();
}

Animation* AnimationSet::getAnimation(const std::string& animationName)
{
	if (!this->_animations.contains(animationName)) throw std::exception("Animation not found");
	return &this->_animations[animationName];
}

const AssimpNodeData& AnimationSet::getRootNode() const
{
	return this->_rootNode;
}

void AnimationSet::readHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = MathConversionUtil::convert(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (int i = 0; i < src->mNumChildren; ++i)
	{
		AssimpNodeData newData;
		this->readHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}