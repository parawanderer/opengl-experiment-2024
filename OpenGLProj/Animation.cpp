#include "Animation.h"

#include <iostream>
#include <assimp/postprocess.h>


Animation::Animation(aiAnimation* animation, Model* model)
{
	std::cout << "Loading animation '" << animation->mName.C_Str() << "'" << std::endl;

	this->_duration = animation->mDuration;
	this->_ticksPerSecond = animation->mTicksPerSecond;
	this->readMissingBones(animation, model);
}

Bone* Animation::findBone(const std::string& name)
{
	const auto iter = std::ranges::find_if(this->_bones, [&](const Bone& bone)
	{
		return bone.getBoneName() == name;
	});
	if (iter == this->_bones.end()) return nullptr;
	return &(*iter);
}

float Animation::getTicksPerSecond() const
{
	return this->_ticksPerSecond;
}

float Animation::getDuration() const
{
	return this->_duration;
}

std::map<std::string, BoneInfo>& Animation::getBoneIdMap()
{
	return this->_boneInfoMap;
}

// this fix is from the guide at https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation for FBX files with missing bones
void Animation::readMissingBones(const aiAnimation* animation, Model* model)
{
	const int size = animation->mNumChannels;

	std::map<std::string, BoneInfo>& boneInfoMap = model->getBoneInfoMap();
	int boneCount = model->getBoneCount();

	// read the bones engaged in an animation and their keyframes
	for (int i =0; i < size; ++ i)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		const std::string boneName = channel->mNodeName.data;

		if (!boneInfoMap.contains(boneName))
		{
			boneInfoMap[boneName] = {
				.id = boneCount
			};
			model->setBoneCount(++boneCount);
		}
		this->_bones.emplace_back(
			boneName,
			boneInfoMap[boneName].id,
			channel
		);
	}

	this->_boneInfoMap = boneInfoMap;
}
