#ifndef ASSIMPNODE_MINE_H
#define ASSIMPNODE_MINE_H

#include <string>
#include <vector>
#include <glm/mat4x4.hpp>

/**
 * \brief Map incoming Assimp library data structure members that we need for animations
 */
struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

#endif