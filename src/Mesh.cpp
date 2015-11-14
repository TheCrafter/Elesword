#include "Mesh.hpp"
#include <array>

//--------------------------------------------------
// Declarations
//--------------------------------------------------

template <typename C>
inline void SetVertexAttribPointers(GLuint BO, const C& container, size_t elementSize, unsigned int numOfElements, unsigned int index);

//--------------------------------------------------
// Public functions
//--------------------------------------------------

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &mVAO);

    std::array<GLuint, 3> BOs = {mPBO, mNBO, mTBO};
    glDeleteBuffers((GLsizei)BOs.size(), BOs.data());
}

void Mesh::Setup()
{
    std::array<GLuint, 3> ar;

    glGenBuffers((GLsizei)ar.size(), ar.data());
    mPBO = ar[0];
    mNBO = ar[1];
    mTBO = ar[2];
    glGenVertexArrays(1, &mVAO);

    glBindVertexArray(mVAO);
    {
        // Set positions
        SetVertexAttribPointers<std::vector<glm::vec3>>(
            mPBO,
            mPositions,
            sizeof(glm::vec3),
            3,
            0);

        // Set normals
        SetVertexAttribPointers<std::vector<glm::vec3>>(
            mNBO,
            mNormals,
            sizeof(glm::vec3),
            3,
            1);

        // Set texture coordinates
        SetVertexAttribPointers<std::vector<glm::vec2>>(
            mTBO,
            mTexCoords,
            sizeof(glm::vec2),
            2,
            2);
    }
    glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader)
{
    shader.Use();
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(mPositions));
    glBindVertexArray(0);
}

//--------------------------------------------------
// Private functions
//--------------------------------------------------

//--------------------------------------------------
// Util functions
//--------------------------------------------------

template <typename C>
inline void SetVertexAttribPointers(GLuint BO, const C& container, size_t elementSize, unsigned int numOfElements, unsigned int index)
{
    glBindBuffer(GL_ARRAY_BUFFER, BO);
    {
        glBufferData(
            GL_ARRAY_BUFFER,
            container.size() * elementSize,
            container.data(),
            GL_STATIC_DRAW);
        glVertexAttribPointer(index, numOfElements, GL_FLOAT, GL_FALSE, numOfElements * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(index);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}