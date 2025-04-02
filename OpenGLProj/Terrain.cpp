#include "Terrain.h"

#include <filesystem>
#include <vector>
#include <glm/ext/matrix_transform.hpp>

#include "Colors.h"
#include "ErrorUtils.h"
#include "FileUtils.h"
#include "ResourceUtils.h"
#include "stb_image.h"

#define RENDER_AS_MESH false

#if RENDER_AS_MESH
#define DEBUG_RENDER_AS_MESH_CONFIG_PRE glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#define DEBUG_RENDER_AS_MESH_CONFIG_POST glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#else
#define DEBUG_RENDER_AS_MESH_CONFIG_PRE 
#define DEBUG_RENDER_AS_MESH_CONFIG_POST 
#endif


const float Terrain::_resizeFactor = 2.0f;

void Terrain::insertNormContribution(unsigned int index0, unsigned int index1, unsigned int index2)
{
	glm::vec3 v1 = this->_vertices[index0].pos - this->_vertices[index1].pos;
	glm::vec3 v2 = this->_vertices[index0].pos - this->_vertices[index2].pos;
	glm::vec3 contrib = glm::cross(v1, v2);

	this->_vertices[index0].normal += contrib;
	this->_vertices[index1].normal += contrib;
	this->_vertices[index2].normal += contrib;
}

/*
 * this builds out the following "tiles":

		  ┌────┬────┬────┐
		  │    │    │    │
		  │ LB │ B  │ RB │
		  ├────┼────┼────┤
		  │    │    │    │
		  │ L  │    │ R  │
		  ├────┼────┼────┤
		  │    │    │    │
		  │ LF │ F  │ RF │
		  └────┴────┴────┘

if the middle "tile" is the main/primary tile

TODO/note:
-	first of all, the current ground pattern we have does not yield itself to forming
	very nice patterns.
	But it looks better than nothing

-	this "tile" system could be optimized by using lower resolution "tiles"/meshes
	(so 8bit heightmaps vs 16bit as I use now) when they are further away.
	If I have some extra time I will implement this, such that based on your current
	tile the tiles further away will be lower-res.
	... You could probably also scale down the meshes with more "steps" as is done in
	modern video games, but I will definitely not do anything this complex for this project.

-	There's currently "seams" in the edges where the tiles meet each other because the
	average vertex normal computation does not include normals at the other side of the tile
	when computing something for one of the edges. The fix is obviously to take normals for all
	the "virtual tiles" that would form seams (so you'd pretend that there's triangles there even
	though there are not).

The main problem I have is that the heightmap that I am using is basically
just the best looking SMOOTH "desert" height map I could get. It doesn't really look very good.
I'd prefer to have a better tiling desert terrain or a generated one that looks closer to real life.
I think THIS might be closer to it: https://stackoverflow.com/questions/24606406/desert-fractal-opengl
But I'm also fairly sure that the games that have nice deserts like Assassin's Creed Origins
(https://assassinscreed.fandom.com/wiki/Desheret_Desert?file=ACO_Desheret_Desert.jpg)
didn't procedurally generate those maps.

All of these also seem hand-crafted/smoother than what you expect from height-maps: https://www.cgtrader.com/3d-models/desert


Sidenote: this right here (3:00) is that exact 8bit "stairs"/minecraft look issue that I had before:
https://www.gdcvault.com/play/1029222/Machine-Learning-Summit-Advanced-Heightmap
So I guess making nice-looking sand dunes isn't as simple as making mountainous terrain

So I'm just going to choose not to go in-depth with this right now because the *sand dunes* generation seems like
its very own problem... I'll just stick with my "plastic" looking map for now.


*/
void Terrain::populateModelMatrices()
{
	// this->_terrainModelL = this->_terrainModel;
	// this->_terrainModelL = glm::translate(this->_terrainModelL, glm::vec3((this->_width - 2), 0.0f, 0.0f));
	// this->_terrainModelL = glm::scale(this->_terrainModelL, glm::vec3(-1.0f, 1.0f, 1.0f));
	// this->_terrainNormalMatrixL = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelL)));
	//
	// this->_terrainModelR = this->_terrainModel;
	// this->_terrainModelR = glm::translate(this->_terrainModelR, glm::vec3(-(this->_width - 2), 0.0f, 0.0f));
	// this->_terrainModelR = glm::scale(this->_terrainModelR, glm::vec3(-1.0f, 1.0f, 1.0f));
	// this->_terrainNormalMatrixR = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelR)));
	//
	// this->_terrainModelF = this->_terrainModel;
	// this->_terrainModelF = glm::translate(this->_terrainModelF, glm::vec3(0.0f, 0.0f, (this->_height - 2)));
	// this->_terrainModelF = glm::scale(this->_terrainModelF, glm::vec3(1.0f, 1.0f, -1.0f));
	// this->_terrainNormalMatrixF = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelF)));
	//
	// this->_terrainModelB = this->_terrainModel;
	// this->_terrainModelB = glm::translate(this->_terrainModelB, glm::vec3(0.0f, 0.0f, -(this->_height)));
	// this->_terrainModelB = glm::scale(this->_terrainModelB, glm::vec3(1.0f, 1.0f, -1.0f));
	// this->_terrainNormalMatrixB = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelB)));
	//
	// this->_terrainModelLF = this->_terrainModel;
	// this->_terrainModelLF = glm::translate(this->_terrainModelLF, glm::vec3((this->_width - 2), 0.0f, (this->_height - 2)));
	// this->_terrainModelLF = glm::scale(this->_terrainModelLF, glm::vec3(-1.0f, 1.0f, -1.0f));
	// this->_terrainNormalMatrixLF = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelLF)));
	//
	// this->_terrainModelRF = this->_terrainModel;
	// this->_terrainModelRF = glm::translate(this->_terrainModelRF, glm::vec3(-(this->_width - 2), 0.0f, (this->_height - 2)));
	// this->_terrainModelRF = glm::scale(this->_terrainModelRF, glm::vec3(-1.0f, 1.0f, -1.0f));
	// this->_terrainNormalMatrixRF = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelRF)));
	//
	// this->_terrainModelLB = this->_terrainModel;
	// this->_terrainModelLB = glm::translate(this->_terrainModelLB, glm::vec3((this->_width - 2), 0.0f, -(this->_height)));
	// this->_terrainModelLB = glm::scale(this->_terrainModelLB, glm::vec3(-1.0f, 1.0f, -1.0f));
	// this->_terrainNormalMatrixLB = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelLB)));
	//
	// this->_terrainModelRB = this->_terrainModel;
	// this->_terrainModelRB = glm::translate(this->_terrainModelRB, glm::vec3(-(this->_width - 2), 0.0f, -(this->_height)));
	// this->_terrainModelRB = glm::scale(this->_terrainModelRB, glm::vec3(-1.0f, 1.0f, -1.0f));
	// this->_terrainNormalMatrixRB = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelRB)));


	_renderMatrices.emplace_back(this->_terrainModel, this->_terrainNormalMatrix);
	// _renderMatrices.emplace_back(this->_terrainModelL, this->_terrainNormalMatrixL);
	// _renderMatrices.emplace_back(this->_terrainModelR, this->_terrainNormalMatrixR);
	// _renderMatrices.emplace_back(this->_terrainModelF, this->_terrainNormalMatrixF);
	// _renderMatrices.emplace_back(this->_terrainModelB, this->_terrainNormalMatrixB);
	// _renderMatrices.emplace_back(this->_terrainModelLF, this->_terrainNormalMatrixLF);
	// _renderMatrices.emplace_back(this->_terrainModelRF, this->_terrainNormalMatrixRF);
	// _renderMatrices.emplace_back(this->_terrainModelLB, this->_terrainNormalMatrixLB);
	// _renderMatrices.emplace_back(this->_terrainModelRB, this->_terrainNormalMatrixRB);
}

void Terrain::generateVerticesFromHeightMap(unsigned short* data, int nChannels, float yScale, float yShift)
{
	const float textureDivScaling = 2.0f;

	int index = 0;
	for (unsigned int z = 0; z < this->_height; ++z)
	{
		for (unsigned int x = 0; x < this->_width; ++x)
		{
			unsigned short* texel = data + (x + this->_width * z) * nChannels;
			unsigned short y = texel[0];

			const float xPos = -this->_width / _resizeFactor + x;
			const float yPos = (float)y * yScale - yShift;
			const float zPos = -this->_height / _resizeFactor + z;

			this->_vertices[index].pos = glm::vec3(
				xPos, // x
				yPos, // y
				zPos // z
			);
			this->_vertices[index].texture = glm::vec2(
				xPos / textureDivScaling,
				zPos / textureDivScaling
			);

			this->_heightMap[x + this->_width * z] = yPos;

			index++;
		}
	}
}

void Terrain::mapTriangles()
{
	for (unsigned int i = 0; i < this->_height - 1; i++)
	{
		for (unsigned int j = 0; j < this->_width - 1; j++)
		{
			// every set of 4 pixels is considered as the vertices.
			// These 4 pixels are split into two triangles

			// first/left triangle:  (clockwise)
			//                  0---2
			//                  | / |
			//                  1----
			const unsigned int index0 = j + (this->_width * i);
			const unsigned int index1 = j + (this->_width * (i + 1));
			const unsigned int index2 = (j + 1) + (this->_width * i);
			this->_indices.push_back(index0); // 0
			this->_indices.push_back(index1); // 1
			this->_indices.push_back(index2); // 2

			this->insertNormContribution(index0, index1, index2); // 3.1 calculate normals for smooth surface rendering (https://www.youtube.com/watch?v=bwq_y0zxpQM&list=PLA0dXqQjCx0S9qG5dWLsheiCJV-_eLUM0&index=6)

			// second/right triangle:  (clockwise)
			//                  ----2
			//                  | / |
			//                  1---3
			const unsigned int index3 = (j + 1) + (this->_width * (i + 1));
			this->_indices.push_back(index1); // 1
			this->_indices.push_back(index3); // 3
			this->_indices.push_back(index2); // 2

			this->insertNormContribution(index1, index3, index2); // 3.1 calculate normals for smooth surface rendering
		}
	}
}


/**
 * inspired by the following articles/videos:
 * https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map (I've chosen not to use this exact approach because I could not figure out how to get a normal using the "strips")
 * https://www.youtube.com/watch?v=bwq_y0zxpQM&list=PLA0dXqQjCx0S9qG5dWLsheiCJV-_eLUM0&index=6,
 * https://www.youtube.com/watch?v=xoqESu9iOUE&list=PLA0dXqQjCx0S9qG5dWLsheiCJV-_eLUM0&index=3
 */
Terrain::Terrain(
	Shader* shader, 
	const std::string& sourceHeightMapPath, // must be 16-bit
	const std::string& texturePath0, 
	const std::string& texturePath1,
	const std::string& textureNormalMap,
	float yScaleMult, 
	float yShift,
	const glm::vec3& sunPos,
	const glm::vec3& sunLightColor)
:
_shader(shader),
_terrainModel(glm::mat4(1.0f)),
_terrainNormalMatrix(glm::mat3(glm::transpose(glm::inverse(_terrainModel))))
{
	int width, height, nChannels;
	assertFileExists(sourceHeightMapPath);
	assertFileExists(texturePath0);
	assertFileExists(texturePath1);
	assertFileExists(textureNormalMap);

	// load 16-bit heightmap image
	unsigned short* data = stbi_load_16(sourceHeightMapPath.c_str(), &width, &height, &nChannels, 0);
	if (width == 1 || height == 1) throw std::exception("Height map must have a width and height dimension of at least 2px");

	// 0. setup
	this->_width = width;
	this->_height = height;
	this->_heightMap.resize(this->_height * this->_width); // fill out to be referenced later
	this->_vertices.resize(this->_width * this->_height); // make space

	this->populateModelMatrices(); // for terrain "tiling"

	// load textures
	this->_textureId0 = loadSRGBColorSpaceTexture(texturePath0.c_str(), PROJ_CURRENT_DIR, GL_TEXTURE0); // texture0
	this->_textureId1 = loadSRGBColorSpaceTexture(texturePath1.c_str(), PROJ_CURRENT_DIR, GL_TEXTURE1);  // texture1
	this->_textureNormalId = loadDataTexture(textureNormalMap.c_str(), PROJ_CURRENT_DIR, GL_TEXTURE2); // texture2 (normal map)

	// vertex generation
	const float yScale = yScaleMult / 65536.0f; // 16-bit image gives more possible levels...

	// 1. generate vertices from the height map. The x and z are evenly spaced on a grid.
	// The height map (color) gives the height of the vertex = the y coordinate
	this->generateVerticesFromHeightMap(data, nChannels, yScale, yShift);
	stbi_image_free(data);

	// 2. the indices array here maps out the vertices composing individual triangles to prevent repetitions
	this->mapTriangles();

	// 3. normalize all the vertex normals (average of all the plane normals that share this vertex. Up to 6 but possibly less!)
	for (unsigned int i = 0; i < this->_vertices.size(); ++i)
	{
		this->_vertices[i].normal = glm::normalize(this->_vertices[i].normal);
	}

	// 4. OpenGL initialization of triangle data
	this->setupMesh();

	// 5. Shader configuration
	this->setupShader(sunPos, sunLightColor);

	glBindVertexArray(0);
}

void Terrain::setupMesh()
{
	glGenVertexArrays(1, &this->_VAO);
	glBindVertexArray(this->_VAO);

	glGenBuffers(1, &this->_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);
	glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(TerrainVertex), &this->_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, texture));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, normal));
	
	// normal texture mapping
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, tangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, normal));
	

	glGenBuffers(1, &this->_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_indices.size() * sizeof(unsigned int), &this->_indices[0], GL_STATIC_DRAW);
}

void Terrain::setupShader(const glm::vec3& sunPos, const glm::vec3& sunLightColor)
{
	this->_shader->use();
	this->_shader->setMat4("model", this->_terrainModel); // model transform (to world coords)
	// matrix to model the normal if there's non-linear scaling going on in the model matrix
	this->_shader->setMat3("normalMatrix", this->_terrainNormalMatrix);
	// material (terrain)
	this->_shader->setInt("material.diffuse", 0); // material (texture references) -- texture0
	this->_shader->setInt("material.ambient", 1); // material (texture references) -- texture1
	this->_shader->setInt("material.normal", 2); // material (texture references) -- texture2
	this->_shader->setVec3("material.specular", glm::vec3(0.949, 0.776, 0.431));
	this->_shader->setFloat("material.shininess", 0.5f);
	// light (sun)
	this->_shader->setVec3("light.position", sunPos);
	this->_shader->setVec3("light.ambient", sunLightColor * 0.4f);
	this->_shader->setVec3("light.diffuse", sunLightColor * 0.9f);
	this->_shader->setVec3("light.specular", sunLightColor * 0.00f);
}

void Terrain::render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos)
{
	this->_shader->use();
	this->_shader->setMat4("view", view);
	this->_shader->setMat4("projection", projection);
	this->_shader->setVec3("viewPos", viewPos);

	DEBUG_RENDER_AS_MESH_CONFIG_PRE

	glBindVertexArray(this->_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->_textureId0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->_textureId1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->_textureNormalId);

	for (std::pair<glm::mat4, glm::mat3>& p : this->_renderMatrices)
	{
		this->_shader->setMat4("model", p.first);
		this->_shader->setMat3("normalMatrix", p.second);
		glDrawElements(GL_TRIANGLES, this->_indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);

	DEBUG_RENDER_AS_MESH_CONFIG_POST
}

float _barycentricWeightPart1(float x0, float y0, float x1, float y1, float x2, float y2)
{
	return (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2);
}

float _barycentricWeightPart2(float x0, float y0, float x1, float y1, float x2, float y2)
{
	return (y2 - y1) * (x0 - x2) + (x1 - x2) * (y0 - y2);
}

float _barycentricWeightPart3(float x0, float y0, float x1, float y1, float x2, float y2)
{
	return (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2);
}

float Terrain::getWorldHeightAt(float x, float z) const
{
	constexpr float allowedError = 0.001f;

	const float xIndex = x + this->_width / _resizeFactor;
	const float zIndex = z + this->_height / _resizeFactor;

	const float xFloor = std::floor(xIndex);
	const float zFloor = std::floor(zIndex);

	const int xFloorInt0 = (int)xFloor;
	const int zFloorInt0 = (int)zFloor;

	if (xFloor < 0 || xFloor >= this->_width || zFloor < 0 || zFloor >= this->_height)
		throw std::exception("Invalid player position (not on grid)"); // must actually be within the map

	// assume that we are standing exactly on the vertex
	if (fabs(xIndex - xFloor) < allowedError && fabs(zIndex - zFloor) < allowedError)
		return this->getWorldHeight(xFloorInt0, zFloorInt0); // resulting value is already scaled/transformed to world scale

	// otherwise we will interpolate the appropriate height across the triangle.
	// (Barycentric Coordinates solution)

	// which triangle?
	//                  0---2   triangle 1
	//                  | / |
	//                  1----
	// or
	//                  ----2   triangle 2
	//                  | / |
	//                  1---3
	// we compute to what side of the line splitting the 2 triangles our point
	// (xIndex, zIndex) lies, using the dot products of "1" and "2"
	const int xFloor1 = xFloorInt0;
	const int zFloor1 = zFloorInt0 + 1;
	const int xFloor2 = xFloorInt0 + 1;
	const int zFloor2 = zFloorInt0;


	const float d = (xIndex - xFloor1) * (zFloor2 - zFloor1) - (zIndex - zFloor1) * (xFloor2 - xFloor1);
	// (d <= 0) -> triangle 1
	// (d > 0)  -> triangle 2

	// interpolate height (= y coordinate) across the triangle
	// (mainly referenced this article https://codeplea.com/triangular-interpolation
	// while implementing but obviously I have read about them separately before)

	// choose triangle 1 (vertex "0") or triangle 2 (vertex "3")
	const int xFloorOpt = d > 0 ? xFloorInt0 : (xFloorInt0 + 1);
	const int zFloorOpt = d > 0 ? zFloorInt0 : (zFloorInt0 + 1);

	// get weights (how much every vertex will "contribute" to the final "height" that a point on this triangle should be at)
	float w1 = _barycentricWeightPart1(xIndex, zIndex, xFloor1, zFloor1, xFloor2, zFloor2)
				/ _barycentricWeightPart1(xFloorOpt, zFloorOpt, xFloor1, zFloor1, xFloor2, zFloor2);

	float w2 = _barycentricWeightPart2(xIndex, zIndex, xFloorOpt, zFloorOpt, xFloor2, zFloor2)
				/ _barycentricWeightPart3(xFloorOpt, zFloorOpt, xFloor1, zFloor1, xFloor2, zFloor2);

	float w3 = 1 - w1 - w2;

	if (w1 < 0 || w2 < 0 || w3 < 0)
		throw std::exception("Point is not inside triangle!");

	// apply weights to results
	return (w1 * this->getWorldHeight(xFloorOpt, zFloorOpt))
		+ (w2 * this->getWorldHeight(xFloor1, zFloor1))
		+ (w3 * this->getWorldHeight(xFloor2, zFloor2));
}

glm::vec3 Terrain::getWorldHeightVecFor(float x, float z) const
{
	return glm::vec3(x, this->getWorldHeightAt(x, z), z);
}

float Terrain::getWorldHeight(int x, int z) const
{
	return this->_heightMap[x + this->_width * z];
}

int Terrain::getWidth() const
{
	return this->_width;
}

int Terrain::getHeight() const
{
	return this->_height;
}
