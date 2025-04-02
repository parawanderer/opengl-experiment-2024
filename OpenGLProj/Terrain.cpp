#include "Terrain.h"

#include <vector>
#include <glad/glad.h>

#include "ErrorUtils.h"
#include "stb_image.h"



/**
 * inspired by the following articles/videos:
 * https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map (I've chosen not to use this exact approach because I could not figure out how to get a normal using the "strips")
 * https://www.youtube.com/watch?v=bwq_y0zxpQM&list=PLA0dXqQjCx0S9qG5dWLsheiCJV-_eLUM0&index=6,
 * https://www.youtube.com/watch?v=xoqESu9iOUE&list=PLA0dXqQjCx0S9qG5dWLsheiCJV-_eLUM0&index=3
 */
Terrain::Terrain(const std::string& sourceHeightMapPath, float yScaleMult, float yShift)
{
	int width, height, nChannels;
	unsigned char* data = stbi_load(sourceHeightMapPath.c_str(), &width, &height, &nChannels, 0);

	if (width == 1 || height == 1)
		throw std::exception("Height map must have a width and height dimension of at least 2px");

	// vertex generation
	const float yScale = yScaleMult / 256.0f;


	this->_vertices.resize(width * height);

	// 1. generate vertices from the height map. The x and z are evenly spaced on a grid.
	// The height map (color) gives the height of the vertex = the y coordinate
	int index = 0;
	for (unsigned int z = 0; z < height; ++z)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			unsigned char* texel = data + (x + width * z) * nChannels; // get the pixel "index"/pointer from the pixel grid in the first colour channel (RED) 
			unsigned char y = texel[0]; // get the value at that pixel (Red value)

			this->_vertices[index].pos = glm::vec3(
				-width / 2.0f + x, // x
				(int)y * yScale - yShift, // y
				-height / 2.0f + z // z
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
			// first/left triangle:
			//                  0---2
			//                  | / |
			//                  1----
			this->_indices.push_back(j + (width * i));				// 0
			this->_indices.push_back(j + (width * (i + 1)));		// 1
			this->_indices.push_back((j + 1) + (width * i));		// 2

			// second/right triangle:
			//                  ----2
			//                  | / |
			//                  1---3
			this->_indices.push_back(j + (width * (i + 1)));		// 1
			this->_indices.push_back((j + 1) + (width * (i + 1))); // 3
			this->_indices.push_back((j + 1) + (width * i));		// 2
		}
	}


	// 3.1 calculate normals for smooth surface rendering (https://www.youtube.com/watch?v=bwq_y0zxpQM&list=PLA0dXqQjCx0S9qG5dWLsheiCJV-_eLUM0&index=6)
	for (unsigned int i = 0; i + 2 < this->_indices.size(); i += 3)
	{
		unsigned int index0 = this->_indices[i];
		unsigned int index1 = this->_indices[i + 1];
		unsigned int index2 = this->_indices[i + 2];
	
		glm::vec3 v1 = this->_vertices[index0].pos - this->_vertices[index2].pos;
		glm::vec3 v2 = this->_vertices[index1].pos - this->_vertices[index2].pos;
		glm::vec3 norm = glm::normalize(glm::cross(v1, v2));
	
		this->_vertices[index0].normal += norm;
		this->_vertices[index1].normal += norm;
		this->_vertices[index2].normal += norm;
	}

	// 3.2 normalize all the vertex normals (average of all the plane normals that share this vertex. Up to 6 but possibly less!)
	for (unsigned int i = 0; i < this->_vertices.size(); ++i)
	{
		this->_vertices[i].normal = glm::normalize(this->_vertices[i].normal);
	}

	glGenVertexArrays(1, &this->_VAO);
	glBindVertexArray(this->_VAO);

	glGenBuffers(1, &this->_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);
	glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(TerrainVertex), &this->_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &this->_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_indices.size() * sizeof(unsigned int), &this->_indices[0], GL_STATIC_DRAW);
}

void Terrain::render()
{
	glBindVertexArray(this->_VAO);
	glDrawElements(GL_TRIANGLES, this->_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
