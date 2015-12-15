#ifndef ELESWORD_ASSIMPLOADER_HPP
#define ELESWORD_ASSIMPLOADER_HPP

#include <vector>
#include <string>
#include <memory>

#define GLEW_STATIC
#include <GL/glew.h>

#include "../Texture.hpp"
#include "../Render/Shader.hpp"
#include "Model.hpp"

class AssimpLoader
{
public:
    std::unique_ptr<ModelData> LoadData(const std::string& filepath);

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
