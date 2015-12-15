#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

void Shader::Init(const std::string& vertexPath, const std::string& fragmentPath)
{
    // Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensures ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);
    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // Convert stream into GLchar array
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();

    // 2. Compile shaders
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << std::endl << infoLog << std::endl;
    };

    // Similiar for Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << std::endl << infoLog << std::endl;
    }

    // Shader Program
    mProgramID = glCreateProgram();
    glAttachShader(mProgramID, vertex);
    glAttachShader(mProgramID, fragment);
    glLinkProgram(mProgramID);
    // Print linking errors if any
    glGetProgramiv(mProgramID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(mProgramID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << std::endl << infoLog << std::endl;
    }

    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::Use() const
{
    glUseProgram(mProgramID);
}

void Shader::LoadView(const glm::mat4& view) const
{
    glUniformMatrix4fv(glGetUniformLocation(GetProgID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
}

void Shader::LoadProjection(const glm::mat4& proj) const
{
    glUniformMatrix4fv(glGetUniformLocation(GetProgID(), "projection"), 1, GL_FALSE, glm::value_ptr(proj));

}

GLuint Shader::GetProgID() const
{
    return mProgramID;
}