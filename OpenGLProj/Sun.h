#ifndef SUN_MINE_H
#define SUN_MINE_H
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "Shader.h"

class Sun
{
public:
	Sun(Shader* shader, const glm::mat4& model, const glm::vec3& sunLightColor);

	void draw();
private:
	Shader* _shader;
	glm::mat4 _model;
	glm::vec3 _sunLightColor;

	unsigned int _VBO;
	unsigned int _VAO;

	static const std::vector<float> _lightCubeVerts;
};

#endif