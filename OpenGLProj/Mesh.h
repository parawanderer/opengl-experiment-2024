#ifndef MESH_MINE_H
#define MESH_MINE_H
#include <map>
#include <vector>

#include <glm/mat4x4.hpp>

#include "Shader.h"
#include "Vertex.h"


#define MAX_NUM_BONES_PER_VERTEX 4	


// We store the id of the texture and its type e.g. a diffuse or specular texture.
struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

struct VertexBoneData : Vertex
{
	int boneIds[MAX_NUM_BONES_PER_VERTEX] = { -1, -1, -1, -1 }; // consider -1 as "unset"
	float weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };
};

struct BoneInfo
{
	// id is index in finalBoneMatrices
	int id;

	// offset matrix transforms vertex from model space to bone space
	glm::mat4 offset;
};

class Mesh
{
public:
	// mesh data
	std::vector<VertexBoneData> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<VertexBoneData> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void draw(Shader& shader);
private:
	// render data
	unsigned int _VAO;
	unsigned int _VBO;
	unsigned int _EBO;

	std::map<std::string, unsigned int> _boneNameToIndexMap;

	void setupMesh();
};
#endif