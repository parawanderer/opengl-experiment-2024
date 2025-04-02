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

struct ModelVertex : Vertex
{
	int boneIds[MAX_NUM_BONES_PER_VERTEX] = { -1, -1, -1, -1 }; // consider -1 as "unset"
	float weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };
	glm::vec3 tangent; // normal mapping purposes: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
	glm::vec3 bitangent; // ^
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
	inline static const std::string TEXTURE_DIFFUSE = "texture_diffuse";
	inline static const std::string TEXTURE_SPECULAR = "texture_specular";
	inline static const std::string TEXTURE_NORMAL = "texture_normal";

	// mesh data
	std::vector<ModelVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<ModelVertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
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