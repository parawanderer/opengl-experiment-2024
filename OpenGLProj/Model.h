#ifndef MODEL_MINE_H
#define MODEL_MINE_H
#include "Mesh.h"
#include "Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class Model
{
public:
	Model(const char* path);
	void draw(Shader& shader);
private:
	std::vector<Texture> _loadedTextures;
	// model data
	std::vector<Mesh> _meshes;
	std::string _directory; // store the directory of the file path that we'll later need when loading textures

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif