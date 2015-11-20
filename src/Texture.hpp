#ifndef ELESWORD_TEXTURE_HPP
#define ELESWORD_TEXTURE_HPP

#include <array>
#include <string>
#include "Config.hpp"

enum class TextureType
{
    DIFFUSE = 0,
    SPECULAR
};

const std::array<std::string, sizeof(TextureType)> TextureTypeNames = {{
        SHADER_TEXTURE_DIFFUSE_PREFIX,
        SHADER_TEXTURE_SPECULAR_PREFIX}};

struct Texture
{
    GLuint      id;
    TextureType type;
    std::string    path;
};

#endif //~ ELESWORD_TEXTURE_HPP