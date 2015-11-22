#ifndef ELESWORD_SIMPLELOADER_HPP
#define ELESWORD_SIMPLELOADER_HPP

#include <vector>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include "../Render/Shader.hpp"
#include "../Texture.hpp"

struct SimpleMesh
{
    unsigned int indicesOffset;     // Starting position in mIndices
    unsigned int indicesNum;        // Number of indices for this mesh

}; //~ AssimpMesh

class SimpleLoader
{
public:
    void LoadData(
        const std::string& filepath,
        GLuint& vao,
        std::vector<GLfloat>& vData,
        std::vector<SimpleMesh>& vMeshes,
        std::vector<GLuint>& vIndices);

    // Getters
    std::vector<GLfloat>& GetVertices();
    std::vector<GLuint>& GetIndices();

private:
    std::vector<GLfloat> mVertices;
    std::vector<GLuint> mIndices;

}; //~ AssimpLoader

class SimplePainter
{
public:
    void DrawMesh(
        const Shader& shader,
        GLuint vao,
        const std::vector<GLuint>::value_type* indices,
        const SimpleMesh& mesh) const;

}; //~ AssimpPainter

#endif //~ ELESWORD_SIMPLELOADER_HPP
