#ifndef QUAD_MINE_H
#define QUAD_MINE_H

class Quad
{
public:
	Quad();

	~Quad();

	void draw(unsigned int bindTexture);

private:
	float _quadVertices[24] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.

		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int _VBO;
	unsigned int _VAO;
};

#endif