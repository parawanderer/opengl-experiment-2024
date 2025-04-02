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
	Terrain(
		Shader* shader, 
		const std::string& sourceHeightMapPath, 
		const std::string& texturePath0, 
		const std::string& texturePath1, 
		float yScaleMult, 
		float yShift, 
		const glm::mat4& terrainModel,
		const glm::vec3& sunPos,
		const glm::vec3& sunLightColor);

	/**
	 * \brief render mesh strip by strip
	 */
	void render(const glm::mat4&view, const glm::mat4& projection, const glm::vec3& viewPos);
private:
	Shader* _shader;
	std::vector<TerrainVertex> _vertices;
	std::vector<unsigned int> _indices;
	unsigned int _VAO;
	unsigned int _VBO;
	unsigned int _EBO;
	unsigned int _textureId0;
	unsigned int _textureId1;

	glm::mat3 _terrainNormalMatrix;

	void _insertNormContribution(unsigned int index0, unsigned int index1, unsigned int index2);
};

#endif