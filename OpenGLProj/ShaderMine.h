#ifndef SHADER_MINE_H
#define SHADER_MINE_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/fwd.hpp>

class ShaderMine
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    ShaderMine(const char* vShaderCode, const char* fShaderCode);

    static ShaderMine fromFiles(const char* vertexPath, const char* fragmentPath);

    static ShaderMine fromSource(const char* vertexShaderCode, const char* fragmentShaderCode);

    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& matrix) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
};
#endif