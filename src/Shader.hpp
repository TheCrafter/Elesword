#ifndef ELESWORD_SHADER_HPP
#define ELESWORD_SHADER_HPP

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

class Shader
{
public:
    // Initialize the shader
    void Init(const std::string& vertexPath, const std::string& fragmentPath);

    // Use the program
    void Use();

    GLuint GetProgID() const;

private:
    // Program ID
    GLuint mProgramID;
};

#endif