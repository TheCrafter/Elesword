#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <array>

#define GLEW_STATIC
#include <GL/glew.h>

#include "../Config.hpp"

enum class TextureType
{
    DIFFUSE = 0,
    SPECULAR
}; //~ TextureType

const std::array<std::string, sizeof(TextureType)> TextureTypeNames = {{
        SHADER_TEXTURE_DIFFUSE_PREFIX,
        SHADER_TEXTURE_SPECULAR_PREFIX}};

struct Texture
{
    GLuint      id;
    TextureType type;
}; //~ Texture

#endif //~ TEXTURE_HPP