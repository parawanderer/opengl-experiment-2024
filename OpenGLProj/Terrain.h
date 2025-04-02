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
	glm::vec3 tangent = glm::vec3(0.0f);
	glm::vec3 biTangent = glm::vec3(0.0f);
};


class Terrain
{
public:
	Terrain(
		Shader* shader, 
		const std::string& sourceHeightMapPath, 
		const std::string& texturePath0, 
		const std::string& texturePath1,
		const std::string& textureNormalMap,
		float yScaleMult, 
		float yShift,
		const glm::vec3& sunPos,
		const glm::vec3& sunLightColor);

	/**
	 * \brief render mesh strip by strip
	 */
	void render(const glm::mat4&view, const glm::mat4& projection, const glm::vec3& viewPos);

	float getWorldHeightAt(float x, float z) const;
	/**
	 * \brief Will produce a vector [x, y, z] by using getWorldHeightAt(x, z) for y.
	 */
	glm::vec3 getWorldHeightVecFor(float x, float z) const;
	int getWidth() const;
	int getHeight() const;
private:
	Shader* _shader;
	int _width;
	int _height;
	std::vector<float> _heightMap;

	std::vector<TerrainVertex> _vertices;
	std::vector<unsigned int> _indices;
	unsigned int _VAO;
	unsigned int _VBO;
	unsigned int _EBO;
	unsigned int _textureId0;
	unsigned int _textureId1;
	unsigned int _textureNormalId;

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

	void populateModelMatrices();

	float getWorldHeight(int x, int z) const;

	void generateVerticesFromHeightMap(unsigned short* data, int nChannels, float yScale, float yShift);
	void mapTriangles();
	void insertNormContribution(unsigned int index0, unsigned int index1, unsigned int index2);
	void insertTangentAndBitangentContribution(unsigned int index0, unsigned int index1, unsigned int index2, unsigned int index3);
	void setupMesh();
	void setupShader(const glm::vec3& sunPos, const glm::vec3& sunLightColor);

	/**
	 * \brief Essentially this: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
	 *
	 * Pair of two tangent-bitangent pairs. For both triangles respectively.
	 *
	 * We assume clockwise ordering here for the triangle vertices and we compute for the following 2 triangles:
	 * - triangle: <1, 2, 3>
	 * - triangle: <1, 3, 4>
	 */
	static std::pair<std::pair<glm::vec3, glm::vec3>, std::pair<glm::vec3, glm::vec3>> computeTangentAndBitangentForTrianglePlane(
		const glm::vec3& p1,
		const glm::vec3& p2,
		const glm::vec3& p3,
		const glm::vec3& p4,
		const glm::vec2& uv1,
		const glm::vec2& uv2,
		const glm::vec2& uv3,
		const glm::vec2& uv4
	);

	/**
	 * \brief Essentially this: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
	 *
	 * Produces a pair of two tangent-bitangent for a single triangle.
	 *
	 * We assume clockwise ordering here for the triangle vertices and we compute for the following triange <1, 2, 3>:
	 * 
	 *                  1---3
	 *                  | / |
	 *                  2----
	 */
	static std::tuple<glm::vec3, glm::vec3> computeTangentAndBitangentForTriangle(
		const glm::vec3& p1,
		const glm::vec3& p2,
		const glm::vec3& p3,
		const glm::vec2& uv1,
		const glm::vec2& uv2,
		const glm::vec2& uv3
	);
};

#endif