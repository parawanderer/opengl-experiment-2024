#ifndef TERRAIN_MINE_H
#define TERRAIN_MINE_H
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Shader.h"

struct TerrainVertex
{
	glm::vec3 pos;
	glm::vec2 texture;
	glm::vec3 normal = glm::vec3(0.0f);
};


class Terrain
{
public:
	Terrain(Shader& shader, const std::string& sourceHeightMapPath, const std::string& texturePath0, const std::string& texturePath1, float yScaleMult, float yShift);

	/**
	 * \brief render mesh strip by strip
	 */
	void render();
private:
	//std::vector<float> _vertices;
	std::vector<TerrainVertex> _vertices;
	std::vector<unsigned int> _indices;
	unsigned int _VAO;
	unsigned int _VBO;
	unsigned int _EBO;
	unsigned int _textureId0;
	unsigned int _textureId1;
};

#endif