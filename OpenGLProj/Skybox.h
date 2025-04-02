#ifndef SKYBOX_MINE_H
#define SKYBOX_MINE_H
#include "Shader.h"

#include <glm/glm.hpp>
#include <vector>

class Skybox
{
public:
	Skybox(Shader* shader, std::vector<std::string>& faces);

	void render(glm::mat4 view, glm::mat4 projection);
private:
	unsigned int _textureId;
	Shader* _shader;
	unsigned int _VBO;
	unsigned int _VAO;
};

#endif