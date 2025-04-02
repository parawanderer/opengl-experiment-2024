#ifndef ANIMATIONMANAGER_MINE_H
#define ANIMATIONMANAGER_MINE_H
#include "AssimpNode.h"
#include "Animation.h"
#include "Model.h"

/**
 * \brief A container for several animations belonging to a given model.
 * Allows retrieving animations by the name defined in the source file for the model.
 */
class AnimationSet
{
public:
	AnimationSet(const std::string& animationPath, Model* model);

	/**
	 * \brief get number of animations for this model
	 */
	int getAnimationCount() const;

	/**
	 * \brief Get animation by name.
	 *
	 * Throws if animation with the given name does not exist.
	 */
	Animation* getAnimation(const std::string& animationName);

	const AssimpNodeData& getRootNode() const;

private:
	std::map<const std::string, Animation> _animations;
	AssimpNodeData _rootNode;

	void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
};

#endif