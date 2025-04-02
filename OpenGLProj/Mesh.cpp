#include "Mesh.h"

#include <utility>

// https://learnopengl.com/Model-Loading/Mesh
Mesh::Mesh(std::vector<ModelVertex> vertices, std::vector<unsigned> indices, std::vector<Texture> textures):
vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures))
{
	this->setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &this->_VAO);
	glGenBuffers(1, &this->_VBO);
	glGenBuffers(1, &this->_EBO);

	glBindVertexArray(this->_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(ModelVertex), &this->vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);


	//vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)0);
	//vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, normal));
	//texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, texCoords));
	// ids
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, MAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(ModelVertex), (void*)offsetof(ModelVertex, boneIds));
	//weights
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, MAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, weights));
	// tangents (normal mapping)
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, tangent));
	// bitangents (normal mapping)
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), (void*)offsetof(ModelVertex, bitangent));

	glBindVertexArray(0);
}

void Mesh::draw(Shader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	bool hasNormal = false;

	for (unsigned int i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		const std::string name = this->textures[i].type;

		if (name == TEXTURE_DIFFUSE) {
			number = std::to_string(diffuseNr++);
		}
		else if (name == TEXTURE_SPECULAR) {
			number = std::to_string(specularNr++);
		}
		else if (name == TEXTURE_NORMAL) {
			number = std::to_string(normalNr++);
			hasNormal = true;
		}

		shader.setInt(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	shader.setBool("material.has_normal", hasNormal);
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(this->_VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
