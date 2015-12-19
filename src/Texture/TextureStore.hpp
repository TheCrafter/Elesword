#ifndef ELESWORD_TEXTURE_STORE_HPP
#define ELESWORD_TEXTURE_STORE_HPP

#include <unordered_map>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include "Texture.hpp"

class TextureStore
{
public:
    /// Default Constructor
    TextureStore();

    /// Disable copy construction
    TextureStore(const TextureStore&) = delete;
    TextureStore& operator=(const TextureStore&) = delete;

    /// Enable move construction
    TextureStore(TextureStore&& other) = default;
    TextureStore& operator=(TextureStore&& other) = default;

    /// Destructor
    ~TextureStore();

    /// Loads to gpu the texture with given filename, if it isn't loaded before
    GLint LoadTexture(const std::string& filepath, bool alpha = false);

private:
    std::unordered_map<std::string, GLuint> mTextures;

}; //~ TextureStore

#endif //~ ELESWORD_TEXTURE_STORE_HPP