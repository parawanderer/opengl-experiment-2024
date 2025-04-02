#include "Model.h"
#include <assimp/postprocess.h>

#include "FileUtils.h"

// https://learnopengl.com/Model-Loading/Model
Model::Model(const char* path)
{
	this->loadModel(path);
}

void Model::draw(Shader& shader)
{
	for (unsigned int i = 0; i < this->_meshes.size(); i++)
	{
		this->_meshes[i].draw(shader);
	}
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, 
		aiProcess_Triangulate // if the model does not (entirely) consist of triangles, it should transform all the model's primitive shapes to triangles first
		| aiProcess_FlipUVs // flips the texture coordinates on the y-axis where necessary during processing
		| aiProcess_GenNormals
		// more interesting options available: https://learnopengl.com/Model-Loading/Model
		// https://assimp.sourceforge.net/lib_html/postprocess_8h.html
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Error::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	this->_directory = path.substr(0, path.find_last_of('/'));
	this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->_meshes.push_back(this->processMesh(mesh, scene)); // this technically flattens the hierarchical relation that was
		// defined by the model's creator which may disallow hierarchical operations (e.g. move all members of a subsection of the hierarchy tree
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex = {
			.position = glm::vec3(
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			),
			.normal = glm::vec3(
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			)
		};

		if (mesh->mTextureCoords[0])
		{
			// does the mesh contain texture coordinates?
			vertex.texCoords = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		} else
		{
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		// process vertex positions, normals and texture coordinates
		vertices.push_back(vertex);
	}

	//process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (unsigned int j = 0; j < this->_loadedTextures.size(); j++)
		{
			if (std::strcmp(this->_loadedTextures[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(this->_loadedTextures[j]); // loaded already
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			// if texture hasn't been loaded already, load it
			Texture texture = {
			.id = loadTextureFromFile(str.C_Str(), this->_directory),
			.type = typeName,
			.path = str.C_Str()
			};
			textures.push_back(texture);
		}
	}
	return textures;
}
