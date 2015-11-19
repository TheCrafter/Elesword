#ifndef ELESWORD_MESH_HPP
#define ELESWORD_MESH_HPP

#include "WarnGuard.hpp"
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
WARN_GUARD_OFF

#include "Shader.hpp"

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