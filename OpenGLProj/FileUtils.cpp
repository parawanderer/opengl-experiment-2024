#include "FileUtils.h"

#include <iostream>

#include "stb_image.h"




unsigned loadTextureFromFile(const char* path, const std::string& directory, std::optional<GLenum> activeTextureUnit)
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
		switch(nrChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			std::cout << "Texture had unexpected number of channels!" << std::endl;
			stbi_image_free(data);
			return -1;
		}

		if (activeTextureUnit.has_value()) glActiveTexture(activeTextureUnit.value());

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

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
