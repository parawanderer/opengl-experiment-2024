#ifndef SPHERE_MINE_H
#define SPHERE_MINE_H
#include <vector>

#include "Shader.h"
#include "Vertex.h"

class Sphere
{
public:
	Sphere(int sectorCount, int stackCount, float radius);

	void draw(Shader& shader);

private:
	int _sectorCount;
	int _stackCount;
	float _radius;

	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;

	unsigned int _VAO;
	unsigned int _VBO;
	unsigned int _EBO;

	void initVertices();
	void initIndices();
	void setupSphere();
};

#endif