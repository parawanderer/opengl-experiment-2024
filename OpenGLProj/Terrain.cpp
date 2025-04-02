#include "Terrain.h"

#include <filesystem>
#include <vector>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>

#include "Colors.h"
#include "ErrorUtils.h"
#include "ResourceUtils.h"
#include "stb_image.h"




void Terrain::_insertNormContribution(unsigned int index0, unsigned int index1, unsigned int index2)
{
	glm::vec3 v1 = this->_vertices[index0].pos - this->_vertices[index1].pos;
	glm::vec3 v2 = this->_vertices[index0].pos - this->_vertices[index2].pos;
	glm::vec3 contrib = glm::cross(v1, v2);

	this->_vertices[index0].normal += contrib;
	this->_vertices[index1].normal += contrib;
	this->_vertices[index2].normal += contrib;
}

unsigned int Terrain::_loadTextureJpg(const char* texturePath, GLenum textureUnit)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(textureUnit); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return -1;
	}
	stbi_image_free(data);

	return texture;
}

void Terrain::_populateModelMatrices()
{
	this->_terrainModelL = this->_terrainModel;
	this->_terrainModelL = glm::translate(this->_terrainModelL, glm::vec3((this->_width - 2), 0.0f, 0.0f));
	this->_terrainModelL = glm::scale(this->_terrainModelL, glm::vec3(-1.0f, 1.0f, 1.0f));
	this->_terrainNormalMatrixL = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelL)));

	this->_terrainModelR = this->_terrainModel;
	this->_terrainModelR = glm::translate(this->_terrainModelR, glm::vec3(-(this->_width - 2), 0.0f, 0.0f));
	this->_terrainModelR = glm::scale(this->_terrainModelR, glm::vec3(-1.0f, 1.0f, 1.0f));
	this->_terrainNormalMatrixR = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelR)));

	this->_terrainModelF = this->_terrainModel;
	this->_terrainModelF = glm::translate(this->_terrainModelF, glm::vec3(0.0f, 0.0f, (this->_height - 2)));
	this->_terrainModelF = glm::scale(this->_terrainModelF, glm::vec3(1.0f, 1.0f, -1.0f));
	this->_terrainNormalMatrixF = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelF)));

	this->_terrainModelB = this->_terrainModel;
	this->_terrainModelB = glm::translate(this->_terrainModelB, glm::vec3(0.0f, 0.0f, -(this->_height)));
	this->_terrainModelB = glm::scale(this->_terrainModelB, glm::vec3(1.0f, 1.0f, -1.0f));
	this->_terrainNormalMatrixB = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelB)));

	this->_terrainModelLF = this->_terrainModel;
	this->_terrainModelLF = glm::translate(this->_terrainModelLF, glm::vec3((this->_width - 2), 0.0f, (this->_height - 2)));
	this->_terrainModelLF = glm::scale(this->_terrainModelLF, glm::vec3(-1.0f, 1.0f, -1.0f));
	this->_terrainNormalMatrixLF = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelLF)));

	this->_terrainModelRF = this->_terrainModel;
	this->_terrainModelRF = glm::translate(this->_terrainModelRF, glm::vec3(-(this->_width - 2), 0.0f, (this->_height - 2)));
	this->_terrainModelRF = glm::scale(this->_terrainModelRF, glm::vec3(-1.0f, 1.0f, -1.0f));
	this->_terrainNormalMatrixRF = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelRF)));

	this->_terrainModelLB = this->_terrainModel;
	this->_terrainModelLB = glm::translate(this->_terrainModelLB, glm::vec3((this->_width - 2), 0.0f, -(this->_height)));
	this->_terrainModelLB = glm::scale(this->_terrainModelLB, glm::vec3(-1.0f, 1.0f, -1.0f));
	this->_terrainNormalMatrixLB = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelLB)));

	this->_terrainModelRB = this->_terrainModel;
	this->_terrainModelRB = glm::translate(this->_terrainModelRB, glm::vec3(-(this->_width - 2), 0.0f, -(this->_height)));
	this->_terrainModelRB = glm::scale(this->_terrainModelRB, glm::vec3(-1.0f, 1.0f, -1.0f));
	this->_terrainNormalMatrixRB = glm::mat3(glm::transpose(glm::inverse(this->_terrainModelRB)));


	_renderMatrices.emplace_back(this->_terrainModel, this->_terrainNormalMatrix);
	_renderMatrices.emplace_back(this->_terrainModelL, this->_terrainNormalMatrixL);
	_renderMatrices.emplace_back(this->_terrainModelR, this->_terrainNormalMatrixR);
	_renderMatrices.emplace_back(this->_terrainModelF, this->_terrainNormalMatrixF);
	_renderMatrices.emplace_back(this->_terrainModelB, this->_terrainNormalMatrixB);
	_renderMatrices.emplace_back(this->_terrainModelLF, this->_terrainNormalMatrixLF);
	_renderMatrices.emplace_back(this->_terrainModelRF, this->_terrainNormalMatrixRF);
	_renderMatrices.emplace_back(this->_terrainModelLB, this->_terrainNormalMatrixLB);
	_renderMatrices.emplace_back(this->_terrainModelRB, this->_terrainNormalMatrixRB);
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
	float yScaleMult, 
	float yShift,
	const glm::mat4& terrainModel,
	const glm::vec3& sunPos,
	const glm::vec3& sunLightColor)
:
_shader(shader),
_terrainModel(terrainModel),
_terrainNormalMatrix(glm::mat3(glm::transpose(glm::inverse(terrainModel))))
{
	int width, height, nChannels;
	assertFileExists(sourceHeightMapPath);
	assertFileExists(texturePath0);
	assertFileExists(texturePath1);


	// load 16-bit heightmap image
	unsigned short* data = stbi_load_16(sourceHeightMapPath.c_str(), &width, &height, &nChannels, 0);
	if (width == 1 || height == 1) throw std::exception("Height map must have a width and height dimension of at least 2px");
	this->_width = width;
	this->_height = height;

	this->_populateModelMatrices();

	// load textures
	this->_textureId0 = _loadTextureJpg(texturePath0.c_str(), GL_TEXTURE0); // texture0
	this->_textureId1 = _loadTextureJpg(texturePath1.c_str(), GL_TEXTURE1); // texture1

	const float textureDivScaling = 2.0f;

	// vertex generation
	const float yScale = yScaleMult / 65536.0f; // 16-bit image gives more possible levels...

	this->_vertices.resize(width * height); // make space

	// 1. generate vertices from the height map. The x and z are evenly spaced on a grid.
	// The height map (color) gives the height of the vertex = the y coordinate
	int index = 0;
	for (unsigned int z = 0; z < height; ++z)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			unsigned short* texel = data + (x + width * z) * nChannels;
			unsigned short y = texel[0];

			const float xPos = -width / 2.0f + x;
			const float zPos = -height / 2.0f + z;

			this->_vertices[index].pos = glm::vec3(
				xPos, // x
				(float)y * yScale - yShift, // y
				zPos // z
			);
			this->_vertices[index].texture = glm::vec2(
				xPos / textureDivScaling,
				zPos / textureDivScaling
			);

			index++;
		}
	}

	stbi_image_free(data);

	// 2. the indices array here maps out the vertices composing individual triangles to prevent repetitions
	for (unsigned int i = 0; i < height - 1; i++)
	{
		for (unsigned int j = 0; j < width - 1; j++)
		{
			// every set of 4 pixels is considered as the vertices.
			// These 4 pixels are split into two triangles
			
			// first/left triangle:  (clockwise)
			//                  0---2
			//                  | / |
			//                  1----
			const unsigned int index0 = j + (width * i);
			const unsigned int index1 = j + (width * (i + 1));
			const unsigned int index2 = (j + 1) + (width * i);
			this->_indices.push_back(index0); // 0
			this->_indices.push_back(index1); // 1
			this->_indices.push_back(index2); // 2

			this->_insertNormContribution(index0, index1, index2); // 3.1 calculate normals for smooth surface rendering (https://www.youtube.com/watch?v=bwq_y0zxpQM&list=PLA0dXqQjCx0S9qG5dWLsheiCJV-_eLUM0&index=6)

			// second/right triangle:  (clockwise)
			//                  ----2
			//                  | / |
			//                  1---3
			const unsigned int index3 = (j + 1) + (width * (i + 1));
			this->_indices.push_back(index1); // 1
			this->_indices.push_back(index3); // 3
			this->_indices.push_back(index2); // 2

			this->_insertNormContribution(index1, index3, index2); // 3.1 calculate normals for smooth surface rendering
		}
	}

	// 3.2 normalize all the vertex normals (average of all the plane normals that share this vertex. Up to 6 but possibly less!)
	for (unsigned int i = 0; i < this->_vertices.size(); ++i)
	{
		//this->_vertices[i].normal = glm::normalize(this->_vertices[i].normal);
		this->_vertices[i].normal = glm::normalize(this->_vertices[i].normal);
	}

	glGenVertexArrays(1, &this->_VAO);
	glBindVertexArray(this->_VAO);

	glGenBuffers(1, &this->_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);
	glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(TerrainVertex), &this->_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &this->_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_indices.size() * sizeof(unsigned int), &this->_indices[0], GL_STATIC_DRAW);

	// configure shader
	this->_shader->use();

	this->_shader->setMat4("model", this->_terrainModel); // model transform (to world coords)
	// matrix to model the normal if there's non-linear scaling going on in the model matrix
	this->_shader->setMat3("normalMatrix", this->_terrainNormalMatrix);
	// material (terrain)
	this->_shader->setInt("material.diffuse", 0); // material (texture references) -- texture0
	this->_shader->setInt("material.ambient", 1); // material (texture references) -- texture1
	this->_shader->setVec3("material.specular", glm::vec3(0.949, 0.776, 0.431));
	this->_shader->setFloat("material.shininess", 1);
	// light (sun)
	this->_shader->setVec3("light.position", sunPos);
	this->_shader->setVec3("light.ambient", sunLightColor * 0.4f);
	this->_shader->setVec3("light.diffuse", sunLightColor * 0.9f);
	this->_shader->setVec3("light.specular", sunLightColor * 0.0f);
}

void Terrain::render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos)
{
	this->_shader->use();
	this->_shader->setMat4("view", view);
	this->_shader->setMat4("projection", projection);
	this->_shader->setVec3("viewPos", viewPos);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(this->_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->_textureId0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->_textureId1);

	for (std::pair<glm::mat4, glm::mat3>& p : this->_renderMatrices)
	{
		this->_shader->setMat4("model", p.first);
		this->_shader->setMat3("normalMatrix", p.second);
		glDrawElements(GL_TRIANGLES, this->_indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
