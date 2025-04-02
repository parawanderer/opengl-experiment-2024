#ifndef MODEL_MINE_H
#define MODEL_MINE_H
#include "Mesh.h"
#include "Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

/**
 * \brief A 3D model container. Contains one or more Meshes.
 */
class Model
{
public:
	Model(const char* path);

	/**
	 * \brief Draw the model with the given shader
	 */
	void draw(Shader& shader);

	std::map<std::string, BoneInfo>& getBoneInfoMap();

	int getBoneCount() const;

	void setBoneCount(int count);

private:
	std::vector<Texture> _loadedTextures;
	// model data
	std::vector<Mesh> _meshes;
	std::string _directory; // store the directory of the file path that we'll later need when loading textures
	// model animation data
	std::map<std::string, BoneInfo> _boneInfoMap;
	int _boneCounter = 0;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	void processBones(aiMesh* mesh, std::vector<ModelVertex>& vertices);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	static void setVertexBoneData(ModelVertex& data, int boneId, float weight);

};

#endif