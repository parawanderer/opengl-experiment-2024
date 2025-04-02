#include "Model.h"

#include <iostream>
#include <map>
#include <assimp/postprocess.h>

#include "ConfigConstants.h"
#include "FileUtils.h"
#include "MathConversionUtil.h"


// https://learnopengl.com/Model-Loading/Model
// https://www.youtube.com/watch?v=r6Yv_mh79PI
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
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
	std::cout << "Loading model: '" << path << "'" << std::endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, 
		aiProcess_Triangulate // if the model does not (entirely) consist of triangles, it should transform all the model's primitive shapes to triangles first
		| aiProcess_FlipUVs // flips the texture coordinates on the y-axis where necessary during processing
		| aiProcess_GenNormals
		| aiProcess_CalcTangentSpace
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
	std::cout << "Processing mesh: '" << mesh->mName.C_Str() << "'" << std::endl;

	std::vector<ModelVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		ModelVertex vertex = {
			{
				.position = MathConversionUtil::convert(mesh->mVertices[i]),
			   .normal = MathConversionUtil::convert(mesh->mNormals[i])
			},
			{ -1, -1, -1, -1 },
			{0.0f, 0.0f, 0.0f, 0.0f },
			MathConversionUtil::convert(mesh->mTangents[i]),
			MathConversionUtil::convert(mesh->mBitangents[i])
		};

		// does the mesh contain texture coordinates?
		vertex.texCoords = mesh->mTextureCoords[0] ? MathConversionUtil::convert(mesh->mTextureCoords[0][i]) : glm::vec2(0.0f, 0.0f);


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

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, Mesh::TEXTURE_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, Mesh::TEXTURE_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_NORMALS, Mesh::TEXTURE_NORMAL); // TODO: need to check if "aiTextureType_NORMALS" is OK or if we have to use "aiTextureType_HEIGHT"
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}


	// process bones
	if (mesh->HasBones())
	{
		this->processBones(mesh, vertices);
	}

	return Mesh(vertices, indices, textures);
}

std::map<std::string, BoneInfo>& Model::getBoneInfoMap()
{
	return this->_boneInfoMap;
}

int Model::getBoneCount() const
{
	return this->_boneCounter;
}

void Model::setBoneCount(int count)
{
	this->_boneCounter = count;
}

void Model::processBones(aiMesh* mesh, std::vector<ModelVertex>& vertices)
{
	std::cout << "Processing " << mesh->mNumBones << " bones" << std::endl;
	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		int boneId = -1;
		aiBone* bone = mesh->mBones[i];
		const std::string boneName = bone->mName.C_Str();
		std::cout << "Processing bone: '" << boneName << "'" << std::endl;

		// store mapping info (bone name (string) to bone data [id + transformation matrix to go from local to bone space for a vertex]
		if (!this->_boneInfoMap.contains(boneName))
		{
			// consider this a new bone
			boneId = this->_boneCounter;
			this->_boneInfoMap[boneName] = {
				.id = this->_boneCounter,
				.offset = MathConversionUtil::convert(bone->mOffsetMatrix)
			};

			this->_boneCounter++;
		}
		else
		{
			// reuse bone (already processed before)
			boneId = this->_boneInfoMap[boneName].id;
		}
		assert(boneId != -1);


		// store weight for bone for every vertex affected by this bone [for transforming the bone with animations]
		aiVertexWeight* weights = bone->mWeights;
		const unsigned int numWeights = bone->mNumWeights;
		for (unsigned int j = 0; j < numWeights; j++)
		{
			const int vertexId = weights[j].mVertexId;
			const float weight = weights[j].mWeight;
			assert(vertexId < vertices.size());
			setVertexBoneData(vertices[vertexId], boneId, weight);
		}
	}
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
			.id = loadTextureFromFile(str.C_Str(), this->_directory, std::nullopt, USE_SRGB_COLORS && typeName != Mesh::TEXTURE_NORMAL),
			.type = typeName,
			.path = str.C_Str()
			};
			textures.push_back(texture);
		}
	}
	return textures;
}

void Model::setVertexBoneData(ModelVertex& data, int boneId, float weight)
{
	for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; ++i)
	{
		if (data.boneIds[i] < 0)
		{
			data.weights[i] = weight;
			data.boneIds[i] = boneId;
			return;
		}
	}
	//std::cout << "Extra influence by bone " << boneId << std::endl;
	//throw std::exception("Tried setting a fourth influencing bone for a vertex!");
}
