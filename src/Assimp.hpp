#ifndef ELESWORD_ASSIMP_HPP
#define ELESWORD_ASSIMP_HPP

#include <vector>
#include <array>
#include <memory>

#define GLEW_STATIC
#include <GL/glew.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Config.hpp"
#include "Shader.hpp"
#include "Model.hpp"

enum class AssimpTextureType
{
    DIFFUSE = 0,
    SPECULAR
};

struct AssimpTexture
{
    GLuint      id;
    AssimpTextureType type;
    aiString    path;
};

const std::array<std::string, sizeof(AssimpTextureType)> TextureTypeNames = {{
        SHADER_TEXTURE_DIFFUSE_PREFIX,
        SHADER_TEXTURE_SPECULAR_PREFIX}};

struct AssimpMesh
{
    std::vector<GLuint> indices;
    std::vector<AssimpTexture> textures;
    GLuint VAO;
};

void AssimpDraw(const Shader& shader, const std::vector<AssimpMesh>* meshes);

std::vector<AssimpMesh>* AssimpLoad(const std::string& filepath);

#endif //~ ELESWORD_ASSIMP_HPP
