#include "Mesh.hpp"

//--------------------------------------------------
// Public functions
//--------------------------------------------------

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
    : mVertices(vertices)
    , mIndices(indices)
    , mTextures(textures)
{
    this->setupMesh();
}

void Mesh::Draw(Shader shader)
{
    // Bind appropriate textures
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;

    // Bind textures
    for(std::vector<Texture>::size_type i = 0; i != mTextures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + (GLuint)i);

        // Retrieve texture number (the N in diffuse_textureN)
        std::string number;
        TextureType type = mTextures[i].type;

        // Transfer GLuint to stream
        switch(type)
        {
            case TextureType::DIFFUSE:
                number += (char)diffuseNr++; 
                break;

            case TextureType::SPECULAR:
                number += (char)specularNr++;
                break;
            default:
                break;
        }

        // Now set the sampler to the correct texture unit
        glUniform1i(
            glGetUniformLocation(shader.GetProgID(), ("material." + TextureTypeNames[(size_t)type] + number).c_str()),
            (GLuint)i
        );

        // And finally bind the texture
        glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
    }

    // Also set each mesh's shininess property to a default value
    // (if you want you could extend this to another mesh property and possibly change this value)
    glUniform1f(glGetUniformLocation(shader.GetProgID(), "material.shininess"), 16.0f);

    // Draw mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // "Unbind" textures
    for(std::vector<Texture>::size_type i = 0; i != mTextures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + (GLuint)i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

//--------------------------------------------------
// Private functions
//--------------------------------------------------
void Mesh::setupMesh()
{
    // Create buffers/arrays
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    {
        // Load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), &mIndices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
        // Vertex Normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
    }
    glBindVertexArray(0);
}