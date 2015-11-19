#ifndef ELESWORD_ASSIMPLOADER_HPP
#define ELESWORD_ASSIMPLOADER_HPP

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

class AssimpLoader
{
public:
    struct AssimpMesh
    {
        std::vector<GLuint> indices;
        std::vector<AssimpTexture> textures;
        GLuint VAO;
    };

    static void AssimpDraw(const Shader& shader, const std::vector<AssimpMesh>* meshes);

    static std::vector<AssimpMesh>* AssimpLoad(const std::string& filepath);

private:
    static std::vector<AssimpMesh>* LoadToGpu(const aiScene* scene, const std::string& filepath);

    static std::vector<AssimpTexture> LoadMaterialTextures(
        aiMaterial* mat,
        aiTextureType type,
        std::string typeName,
        std::string assetRootDir,
        std::vector<AssimpTexture> loadedTextures);
}; //~ AssimpLoader

#endif //~ ELESWORD_ASSIMPLOADER_HPP
