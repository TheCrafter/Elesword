#include "SimpleLoader.hpp"
#include <iostream>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <SOIL.h>

#include "../Config.hpp"

//--------------------------------------------------
// SimpleLoader
//--------------------------------------------------
void SimpleLoader::LoadData(
    const std::string& filepath,
    GLuint& vao,
    std::vector<GLfloat>& vData,
    std::vector<SimpleMesh>& vMeshes,
    std::vector<GLuint>& vIndices)
{
    // Unused parameters
    (void)filepath;

    // Load data (mVertices and mIndices should be filled already)
    vData.insert(vData.end(), mVertices.begin(), mVertices.end());
    vIndices.insert(vIndices.end(), mIndices.begin(), mIndices.end());

    SimpleMesh newMesh;
    newMesh.indicesOffset = 0;
    newMesh.indicesNum = (unsigned int)mIndices.size();
    vMeshes.push_back(newMesh);

    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        {
            // Bind data
            glBufferData(
                GL_ARRAY_BUFFER,
                vData.size() * sizeof(GLfloat),
                vData.data(),
                GL_STATIC_DRAW);

            // Vertices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            vIndices.size() * sizeof(GLuint),
            vIndices.data(),
            GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
}

std::vector<GLfloat>& SimpleLoader::GetVertices() { return mVertices; }
std::vector<GLuint>& SimpleLoader::GetIndices() { return mIndices; }

//--------------------------------------------------
// SimplePainter
//--------------------------------------------------
void SimplePainter::DrawMesh(
    const Shader& shader,
    GLuint vao,
    const std::vector<GLuint>::value_type* indices,
    const SimpleMesh& mesh) const
{
    shader.Use();

    // Draw mesh
    glBindVertexArray(vao);
    glDrawElements(
        GL_TRIANGLES,
        mesh.indicesNum,
        GL_UNSIGNED_INT,
        indices);
    glBindVertexArray(0);
}
