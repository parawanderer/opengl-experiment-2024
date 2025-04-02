#ifndef SHADER_MINE_H
#define SHADER_MINE_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/fwd.hpp>

/**
 * \brief My spin on the Shader utility wrapper as given in the tutorial
 */
class Shader
{
public:
    // the program ID
    unsigned int ID;

    /**
     * Build a shader
     * \param vShaderCode vertex shader source code string
     * \param fShaderCode fragment shader source code string
     */
    Shader(const char* vShaderCode, const char* fShaderCode);

    /**
     * Build a shader from the specified shader files at the given paths
     * \param vertexPath file path to shader
     * \param fragmentPath file path to fragment
     * \return Instance of Shader
     */
    static Shader fromFiles(const char* vertexPath, const char* fragmentPath);

    /**
     * The same as Shader()
     */
    static Shader fromSource(const char* vertexShaderCode, const char* fragmentShaderCode);

    /**
     * \brief Makes use the shader/programs associated with the shader for now.
     * Required before calling any of the setX functions 
     */
    void use();
    
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& matrix) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;
};
#endif