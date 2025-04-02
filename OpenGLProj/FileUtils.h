#ifndef FILEUTILS_MINE_H
#define FILEUTILS_MINE_H
#include <string>
#include <optional>

#include <glad/glad.h>

const std::string PROJ_CURRENT_DIR = ".";

/**
 * \brief General texture loader
 *
 * \param path					Path to the file
 * \param directory				Base directory that the file resides in
 * \param activeTextureUnit		what texture unit to load it as. Default is not provided: will not be loaded into a texture unit.
 *
 * \param loadAsSRGB			Whether to load as sRGB texture. This is needed when we want to render the final result using glEnable(GL_FRAMEBUFFER_SRGB);
 *								in the main loops, which gives better looking images when using advanced algorithms to render better looking scenery.
 *								- See: https://learnopengl.com/Advanced-Lighting/Gamma-Correction
 *								- This is relevant for textures such as: diffuse textures ("albedo"/base color)!
 *								- This is not relevant for data textures such as: specular maps and normal maps!
 *
 *
 * \return						id of the new texture
 */
unsigned int loadTextureFromFile(const char* path, const std::string& directory = "", std::optional<GLenum> activeTextureUnit = std::nullopt, bool loadAsSRGB = false);


/**
 * \brief Load a texture that is meant to be used for color data. E.g. mesh base color texture
 *
 * Convenience wrapper for loadTextureFromFile() to make the use case more clear
 */
unsigned int loadSRGBColorSpaceTexture(const char* path, const std::string& directory = "", std::optional<GLenum> activeTextureUnit = std::nullopt);


/**
 * \brief Load a texture that is meant to be used for its data. E.g. mesh normal map texture
 *
 * Convenience wrapper for loadTextureFromFile() to make the use case more clear
 */
unsigned int loadDataTexture(const char* path, const std::string& directory = "", std::optional<GLenum> activeTextureUnit = std::nullopt);

#endif