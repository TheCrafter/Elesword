#ifndef ELESWORD_ASSIMPLOADER_HPP
#define ELESWORD_ASSIMPLOADER_HPP

#include <vector>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include "../Texture.hpp"
#include "../Render/Shader.hpp"
#include "Model.hpp"

class AssimpLoader
{
public:
    bool LoadData(
        const std::string& filepath,
        GLuint& vao,
        GLuint& vbo,
        std::vector<GLfloat>& vData,
        std::vector<Mesh>& vMeshes);

}; //~ AssimpLoader

class AssimpPainter
{
public:
    void DrawMesh(
        const Shader& shader,
        GLuint vao,
        const Mesh& mesh) const;

}; //~ AssimpPainter

#endif //~ ELESWORD_ASSIMPLOADER_HPP
