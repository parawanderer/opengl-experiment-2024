#ifndef MESH_MINE_H
#define MESH_MINE_H
#include "Shader.h"

#include <vector>

#include "Vertex.h"


// We store the id of the texture and its type e.g. a diffuse or specular texture.
struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void draw(Shader& shader);
private:
	// render data
	unsigned int _VAO;
	unsigned int _VBO;
	unsigned int _EBO;

	void setupMesh();
};
#endif