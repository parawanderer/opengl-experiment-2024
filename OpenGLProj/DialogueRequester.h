#ifndef DIALOGUEREQUESTER_MINE_H
#define DIALOGUEREQUESTER_MINE_H
#include <glm/vec3.hpp>

/**
 * \brief An entity that has a dynamic position (i.e. it moves around in the world) that it can provide when asked
 */
class DialogueRequester
{
public:
	virtual glm::vec3 getCurrentPosition() const = 0;
};


#endif