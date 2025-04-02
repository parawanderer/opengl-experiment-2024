#include "FileUtils.h"

#include <iostream>

#include "ConfigConstants.h"
#include "stb_image.h"




unsigned loadTextureFromFile(const char* path, const std::string& directory, std::optional<GLenum> activeTextureUnit, bool loadAsSRGB)
{
	std::string fileName(path);
	fileName = directory + '/' + fileName;

	unsigned int textureId = -1;
	glGenTextures(1, &textureId);

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		GLint internalFormat;
		switch(nrChannels)
		{
		case 1:
			internalFormat = GL_RED;
			format = GL_RED;
			break;
		case 3:
			internalFormat = loadAsSRGB ? GL_SRGB : GL_RGB;
			format = GL_RGB;
			break;
		case 4:
			internalFormat = loadAsSRGB ? GL_SRGB_ALPHA : GL_RGBA;
			format = GL_RGBA;
			break;
		default:
			std::cout << "Texture had unexpected number of channels!" << std::endl;
			stbi_image_free(data);
			return -1;
		}

		if (activeTextureUnit.has_value()) glActiveTexture(activeTextureUnit.value());

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture (at '" << fileName << "')" << std::endl;
	}

	stbi_image_free(data);

	return textureId;
}

unsigned loadSRGBColorSpaceTexture(const char* path, const std::string& directory,
	std::optional<GLenum> activeTextureUnit)
{
	return loadTextureFromFile(path, directory, activeTextureUnit, USE_SRGB_COLORS && true); // use gamma correction if needed (only if we use sRGB rendering globally)
}

unsigned loadDataTexture(const char* path, const std::string& directory, std::optional<GLenum> activeTextureUnit)
{
	return loadTextureFromFile(path, directory, activeTextureUnit, false); // does not get loaded as SRGB as it only contains data (does not need gamma correction)
}
