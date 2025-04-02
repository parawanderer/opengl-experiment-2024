#ifndef FILEUTILS_MINE_H
#define FILEUTILS_MINE_H
#include <string>
#include <optional>

#include <glad/glad.h>

const std::string PROJ_CURRENT_DIR = ".";

unsigned int loadTextureFromFile(const char* path, const std::string& directory = "", std::optional<GLenum> activeTextureUnit = std::nullopt);

#endif