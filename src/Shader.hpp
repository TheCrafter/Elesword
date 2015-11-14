#pragma once

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

class Shader
{
public:
    // Initialize the shader
    void Init(const std::string& vertexPath, const std::string& fragmentPath);

    // Use the program
    void Use() const;

    GLuint GetProgID() const;

private:
    // Program ID
    GLuint mProgramID;
};
