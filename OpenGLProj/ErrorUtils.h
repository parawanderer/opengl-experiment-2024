#ifndef ERRORUTILS_MINE_H
#define ERRORUTILS_MINE_H
#include <glad/glad.h>

// error utility
GLenum glCheckError_(const char* file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#endif