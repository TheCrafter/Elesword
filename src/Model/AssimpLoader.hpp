#ifndef ELESWORD_ASSIMPLOADER_HPP
#define ELESWORD_ASSIMPLOADER_HPP

#include <vector>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include "../Texture.hpp"
#include "../Render/Shader.hpp"

struct AssimpMesh
{
    GLuint ebo;                     // EBO for this mesh
    std::vector<GLuint> indices;    // Indices of this mesh
    std::vector<Texture> textures;  // Textures of this mesh
    unsigned int dataOffset;        // Starting position in mData

}; //~ AssimpMesh

class AssimpLoader
{
public:
    void LoadData(
        const std::string& filepath,
        GLuint& vao,
        std::vector<GLfloat>& vData,
        std::vector<AssimpMesh>& vMeshes);

}; //~ AssimpLoader

class AssimpPainter
{
public:
    void DrawMesh(
        const Shader& shader,
        GLuint vao,
        const AssimpMesh& mesh) const;

}; //~ AssimpPainter

#endif //~ ELESWORD_ASSIMPLOADER_HPP
