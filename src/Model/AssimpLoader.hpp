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
    unsigned int dataOffset;        // Starting position in mData
    unsigned int indicesOffset;     // Starting position in mIndices
    unsigned int indicesNum;        // Number of indices for this mesh
    std::vector<Texture> textures;  // Textures of this mesh

}; //~ AssimpMesh

class AssimpLoader
{
public:
    void LoadData(
        const std::string& filepath,
        GLuint& vao,
        std::vector<GLfloat>& vData,
        std::vector<AssimpMesh>& vMeshes,
        std::vector<GLuint>& vIndices);

}; //~ AssimpLoader

class AssimpPainter
{
public:
    void DrawMesh(
        const Shader& shader,
        GLuint vao,
        const std::vector<GLuint>::value_type* indices,
        const AssimpMesh& mesh) const;

}; //~ AssimpPainter

#endif //~ ELESWORD_ASSIMPLOADER_HPP
