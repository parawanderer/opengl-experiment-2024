#include "Mesh.h"

// https://learnopengl.com/Model-Loading/Mesh
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices, std::vector<Texture> textures):
vertices(vertices), indices(indices), textures(textures)
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

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);


	//vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	//vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	//texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void Mesh::draw(Shader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	for (unsigned int i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = this->textures[i].type;
		if (name == "texture_diffuse") number = std::to_string(diffuseNr++);
		else if (name == "texture_specular") number = std::to_string(specularNr++);

		shader.setInt(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// draw mesh
	glBindVertexArray(this->_VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
