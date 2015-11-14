#ifndef ELESWORD_MESH_HPP
#define ELESWORD_MESH_HPP

#include <vector>
#include "Shader.hpp"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#pragma warning(push)
#pragma warning(disable:4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

struct Vertice
{
    glm::vec3 position;
    glm::vec3 normal;
};

class Mesh
{
public:
    std::vector<glm::vec3> mPositions;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mTexCoords;

    // Destructor
    ~Mesh();

    // Sets vertex attribute pointers
    void Setup();

    // Draw triangles
    void Draw(const Shader& shader);

private:
    GLuint mVAO, // Vertex Array Object
           mPBO, // Position Buffer Object
           mNBO, // Normal   Buffer Object
           mTBO; // Texture  Buffer Object

}; //~ Mesh

#endif //~ ELESWORD_MESH_HPP