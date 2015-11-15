#ifndef ELESWORD_MODEL_HPP
#define ELESWORD_MODEL_HPP

#include <string>
#include <vector>
#include <array>
#include <functional>

#pragma warning(push)
#pragma warning(disable:4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Config.hpp"
#include "Shader.hpp"

class Model
{
public:
    using LoadTextureCb = std::function<GLint(const std::string& path)>;

    enum class TextureType
    {
        DIFFUSE = 0,
        SPECULAR
    };

    struct Texture
    {
        GLuint      id;
        TextureType type;
        aiString    path;
    };
    const static std::array<std::string, sizeof(TextureType)> TextureTypeNames;

    struct Mesh
    {
        std::vector<GLuint> indices;
        std::vector<Texture> textures;
        GLuint VAO;
    };

    // Constructor
    Model(const std::string& filepath, LoadTextureCb cb);

    void Draw(const Shader& shader);

    // Set the callback function to load a texture from a file
    void SetLoadTextureFromFileCb(LoadTextureCb cb);

private:
    std::vector<Mesh> mMeshes;
    std::vector<Texture> mLoadedTextures;
    std::string mFilepath;

    LoadTextureCb mLoadTextureFromFileCb;

    // Loads model to GPU
    void LoadToGpu(const aiScene* scene);

    // Loads textures from a material
    std::vector<Texture> LoadMaterialTextures(
        aiMaterial* mat,
        aiTextureType type,
        std::string typeName);

}; //~ Model

#endif //~ ELESWORD_MODEL_HPP