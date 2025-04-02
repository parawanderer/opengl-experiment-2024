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
	int _width;
	int _height;
	std::vector<TerrainVertex> _vertices;
	std::vector<unsigned int> _indices;
	unsigned int _VAO;
	unsigned int _VBO;
	unsigned int _EBO;
	unsigned int _textureId0;
	unsigned int _textureId1;

	glm::mat4 _terrainModel;
	glm::mat3 _terrainNormalMatrix;
	glm::mat4 _terrainModelL;
	glm::mat3 _terrainNormalMatrixL;
	glm::mat4 _terrainModelR;
	glm::mat3 _terrainNormalMatrixR;
	glm::mat4 _terrainModelB;
	glm::mat3 _terrainNormalMatrixB;
	glm::mat4 _terrainModelF;
	glm::mat3 _terrainNormalMatrixF;
	glm::mat4 _terrainModelLF;
	glm::mat3 _terrainNormalMatrixLF;
	glm::mat4 _terrainModelRF;
	glm::mat3 _terrainNormalMatrixRF;
	glm::mat4 _terrainModelLB;
	glm::mat3 _terrainNormalMatrixLB;
	glm::mat4 _terrainModelRB;
	glm::mat3 _terrainNormalMatrixRB;


	std::vector<std::pair<glm::mat4, glm::mat3>> _renderMatrices;

	void _insertNormContribution(unsigned int index0, unsigned int index1, unsigned int index2);
	static unsigned int _loadTextureJpg(const char* texturePath, GLenum textureUnit);
	void _populateModelMatrices();
};

#endif