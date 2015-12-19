#ifndef ELESWORD_ASSIMPLOADER_HPP
#define ELESWORD_ASSIMPLOADER_HPP

#include <vector>
#include <string>
#include <memory>

#define GLEW_STATIC
#include <GL/glew.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Model.hpp"
#include "../Render/Shader.hpp"
#include "../Texture/TextureStore.hpp"

class AssimpLoader
{
public:
    /// Constructor
    AssimpLoader(TextureStore* texStore);

    std::unique_ptr<ModelData> LoadData(const std::string& filepath);

private:
    TextureStore* mTextureStore;

    std::vector<Texture> LoadMaterialTextures(
        aiMaterial* mat,
        aiTextureType type,
        std::string typeName,
        std::string assetRootDir);
}; //~ AssimpLoader

class AssimpPainter
{
public:
    void DrawMesh(
        const Shader& shader,
        GLuint vao,
        const Mesh& mesh) const;

}; //~ AssimpPainter

#endif //~ ELESWORD_ASSIMPLOADER_HPP
