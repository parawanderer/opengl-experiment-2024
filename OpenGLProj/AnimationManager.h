#ifndef ANIMATIONMANAGER_MINE_H
#define ANIMATIONMANAGER_MINE_H
#include "Animation.h"
#include "Model.h"

class AnimationManager
{
public:
	AnimationManager(const std::string& animationPath, Model* model);

	int getAnimationCount() const;
	Animation* getAnimation(const std::string& animationName);

	const AssimpNodeData& getRootNode() const;

private:
	std::map<const std::string, Animation> _animations;
	AssimpNodeData _rootNode;

	void readHierarchyData(AssimpNodeData& dest, const aiNode* src);

	Animation* getRandomIdle();
};

#endif