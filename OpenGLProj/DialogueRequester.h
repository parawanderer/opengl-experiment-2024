#ifndef DIALOGUEREQUESTER_MINE_H
#define DIALOGUEREQUESTER_MINE_H
#include <glm/vec3.hpp>

class DialogueRequester
{
public:
	virtual glm::vec3 getCurrentPosition() const = 0;
};


#endif