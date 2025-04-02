#include "Terrain.h"

#include <vector>
#include <glad/glad.h>

#include "stb_image.h"

/**
 * derived from https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map
 */
Terrain::Terrain(const std::string& sourceHeightMapPath, float yScaleMult, float yShift)
{
	int width, height, nChannels;
	unsigned char* data = stbi_load(sourceHeightMapPath.c_str(), &width, &height, &nChannels, 0);
	// vertex generation
	const float yScale = yScaleMult / 256.0f;

	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			// retrieve texel for (i, j) tex coord
			unsigned char* texel = data + (j + width * i) * nChannels;
			// raw height at coordinate
			unsigned char y = texel[0];

			// vertex
			this->_vertices.push_back(-height / 2.0f + i); // v.x
			float finalY = (int)y * yScale - yShift;
			this->_vertices.push_back(finalY); // v.y
			this->_vertices.push_back(-width / 2.0f + j); // v.z
		}
	}
	stbi_image_free(data);

	for (unsigned int i = 0; i < height - 1; i++)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			for (unsigned int k = 0; k < 2; ++k)
			{
				this->_indices.push_back(j + width * (i + k));
			}
		}
	}

	// normals

	this->_numStrips = height - 1;
	this->_numVertsPerStrip = width * 2;

	glGenVertexArrays(1, &this->_VAO);
	glBindVertexArray(this->_VAO);

	glGenBuffers(1, &this->_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);
	glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(float), &this->_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &this->_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_indices.size() * sizeof(unsigned int), &this->_indices[0], GL_STATIC_DRAW);
}

void Terrain::render()
{
	glBindVertexArray(this->_VAO);
	for (unsigned int strip = 0; strip < this->_numStrips; ++strip)
	{
		glDrawElements(GL_TRIANGLE_STRIP, this->_numVertsPerStrip, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * this->_numVertsPerStrip * strip));
	}
}
