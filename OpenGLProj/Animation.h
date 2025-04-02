#ifndef ANIMATION_MINE_H
#define ANIMATION_MINE_H
#include <string>

#include "Bone.h"
#include "Model.h"


/**
 * \brief A single animation belonging to a model (composed of one or more meshes). sMain reference: https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
 */
class Animation
{
public:
	Animation() = default;
	~Animation() = default;

	Animation(aiAnimation* animation, Model* model);

	Bone* findBone(const std::string& name);

	float getTicksPerSecond() const;
	float getDuration() const;

	std::map<std::string, BoneInfo>& getBoneIdMap();

private:
	float _duration; // animation duration
	int _ticksPerSecond;
	std::vector<Bone> _bones;
	std::map<std::string, BoneInfo> _boneInfoMap;


	void readMissingBones(const aiAnimation* animation, Model* model);
};

#endif