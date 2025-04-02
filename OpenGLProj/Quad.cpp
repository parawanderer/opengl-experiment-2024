#include "Quad.h"

#include <glad/glad.h>

Quad::Quad()
{
	glGenBuffers(1, &this->_VBO);
	glGenVertexArrays(1, &this->_VAO);
	glBindVertexArray(this->_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->_quadVertices), &this->_quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

Quad::~Quad()
{
	glDeleteVertexArrays(1, &this->_VAO);
	glDeleteBuffers(1, &this->_VBO);
}

void Quad::draw(unsigned int bindTexture)
{
	glBindVertexArray(this->_VAO);
	glBindTexture(GL_TEXTURE_2D, bindTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
