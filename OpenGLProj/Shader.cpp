#include "Shader.h"

#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include "ResourceUtils.h"

#define TYPE_VERTEX_STR "VERTEX"
#define TYPE_FRAGMENT_STR "FRAGMENT"
#define TYPE_GEOMETRY_STR "GEOMETRY"
#define ERROR_BUFFER_SIZE 512

# define GET_LOCATION glGetUniformLocation(this->ID, name.c_str())


unsigned int Shader::compileShader(const char *shaderSourceCode, GLenum type)
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
    		<< (type == GL_VERTEX_SHADER ? TYPE_VERTEX_STR : (type == GL_FRAGMENT_SHADER ? TYPE_FRAGMENT_STR : TYPE_GEOMETRY_STR))
    		<< "::COMPILATION_FAILED\n" << infoLog << std::endl;
        return -1;
    }

    return shaderId;
}

void Shader::checkLinkSuccess(GLuint shaderProgramId)
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

std::string Shader::readFile(const std::string& fileName)
{
    std::string content;
    std::ifstream file;
    // ensure ifstream objects can throw exceptions:
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        file.open(fileName);
        std::stringstream stream;
        // read file's buffer contents into streams
        stream << file.rdbuf();
        // close file handlers
        file.close();
        // convert stream into string
        content = stream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: '" << fileName << '\'' << std::endl;
        return "";
    }
    return content;
}


Shader Shader::fromFiles(const char* vertexPath, const char* fragmentPath, const char* geomShaderPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    assertFileExists(vertexPath);
	assertFileExists(fragmentPath);
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);
    std::string geomShaderCode = "";
    if (geomShaderPath != nullptr) {
        assertFileExists(geomShaderPath);
        geomShaderCode = readFile(geomShaderPath);
    }

	if (vertexCode.empty() || fragmentCode.empty())
        throw new std::exception("Shader source could not be read");

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geomShaderCode.empty() ? nullptr : geomShaderCode.c_str();

    return Shader(vShaderCode, fShaderCode, gShaderCode);
}

Shader Shader::fromSource(const char* vertexShaderCode, const char* fragmentShaderCode, const char* geomShaderCode)
{
    return Shader(vertexShaderCode, fragmentShaderCode, geomShaderCode);
}


Shader::Shader(const char* vShaderCode, const char* fShaderCode, const char* geomShaderCode)
{
    if (std::string(vShaderCode).find(' ') == std::string::npos)
    {
        std::cout << "Most likely invalid shader source code supplied. Did you mean to use Shader::fromFiles()?" << std::endl;
    }

    // compile shaders
    unsigned int vertex = compileShader(vShaderCode, GL_VERTEX_SHADER);
    unsigned int fragment = compileShader(fShaderCode, GL_FRAGMENT_SHADER);
    unsigned int geom = geomShaderCode != nullptr ? compileShader(geomShaderCode, GL_GEOMETRY_SHADER) : 0;

    // shader Program
    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertex);
    glAttachShader(this->ID, fragment);
    if (geom != 0) glAttachShader(this->ID, geom);
    glLinkProgram(ID);
    // print linking errors if any
    checkLinkSuccess(this->ID);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geom != 0) glDeleteShader(geom);
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

void Shader::setMat3(const std::string& name, const glm::mat3& matrix) const
{
    glUniformMatrix3fv(GET_LOCATION, 1, GL_FALSE, &matrix[0][0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) const
{
    glUniform3fv(GET_LOCATION, 1, &vec[0]);
}