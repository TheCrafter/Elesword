#ifndef ELESWORD_MESH_HPP
#define ELESWORD_MESH_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>

#define GLEW_STATIC
#include <GL/glew.h>

#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(pop)


#include <assimp/postprocess.h>

#include "Config.hpp"
#include "Shader.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

enum class TextureType
{
    DIFFUSE = 0,
    SPECULAR
};
const static std::array<std::string, sizeof(TextureType)> TextureTypeNames{{
    SHADER_TEXTURE_DIFFUSE_PREFIX,
    SHADER_TEXTURE_SPECULAR_PREFIX}};

struct Texture
{
    GLuint      id;
    TextureType type;
    aiString    path;
};

class Mesh
{
public:
    std::vector<Vertex>  mVertices;
    std::vector<GLuint>  mIndices;
    std::vector<Texture> mTextures;

    // Constructor
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

    // Render the mesh
    void Draw(Shader shader);

private:
    GLuint VAO, VBO, EBO;

    // Initializes all the buffer objects/arrays
    void setupMesh();
}; //~ Mesh

#endif //~ ELESWORD_MESH_HPP