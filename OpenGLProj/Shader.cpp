#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#define TYPE_VERTEX_STR "VERTEX"
#define TYPE_FRAGMENT_STR "FRAGMENT"
#define ERROR_BUFFER_SIZE 512

# define GET_LOCATION glGetUniformLocation(this->ID, name.c_str())


unsigned int _compileShader(const char *shaderSourceCode, GLenum type)
{
    int success;
    const unsigned int shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &shaderSourceCode, NULL);
    glCompileShader(shaderId);
    // print compile errors if any
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
	    char infoLog[ERROR_BUFFER_SIZE];
	    glGetShaderInfoLog(shaderId, ERROR_BUFFER_SIZE, NULL, infoLog);
        std::cout << "ERROR::SHADER::"
    		<< (type == GL_VERTEX_SHADER ? TYPE_VERTEX_STR : TYPE_FRAGMENT_STR)
    		<< "::COMPILATION_FAILED\n" << infoLog << std::endl;
        return -1;
    }

    return shaderId;
}

void _checkLinkSuccess(GLuint shaderProgramId)
{
    int  success;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if (!success) 
    {
	    char infoLog[ERROR_BUFFER_SIZE];
	    glGetProgramInfoLog(shaderProgramId, ERROR_BUFFER_SIZE, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::CREATION_FAILED\n" << infoLog << std::endl;
    }
}


Shader Shader::fromFiles(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    return Shader(vShaderCode, fShaderCode);
}

Shader Shader::fromSource(const char* vertexShaderCode, const char* fragmentShaderCode)
{
    return Shader(vertexShaderCode, fragmentShaderCode);
}


Shader::Shader(const char* vShaderCode, const char* fShaderCode)
{
    // compile shaders
    unsigned int vertex = _compileShader(vShaderCode, GL_VERTEX_SHADER);
    unsigned int fragment = _compileShader(fShaderCode, GL_FRAGMENT_SHADER);

    // shader Program
    this->ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    _checkLinkSuccess(this->ID);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(this->ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(GET_LOCATION, (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(GET_LOCATION, value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(GET_LOCATION, value);
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix) const
{
    glUniformMatrix4fv(GET_LOCATION, 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) const
{
    glUniform3fv(GET_LOCATION, 1, &vec[0]);
}
