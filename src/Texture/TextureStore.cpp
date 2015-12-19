#include "TextureStore.hpp"
#include <SOIL.h>

//--------------------------------------------------
// Util functions
//--------------------------------------------------
GLint TextureFromFile(const std::string& path, bool alpha)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Load image
    int width, height;
    unsigned char* image =
        SOIL_load_image(path.c_str(), &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);

    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        alpha ? GL_RGBA : GL_RGB,
        width,
        height,
        0,
        alpha ? GL_RGBA : GL_RGB,
        GL_UNSIGNED_BYTE,
        image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    // Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders.
    // Due to interpolation it takes value from next repeat 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    SOIL_free_image_data(image);
    return textureID;
}

//--------------------------------------------------
// public functions
//--------------------------------------------------
TextureStore::TextureStore()
{
}

TextureStore::~TextureStore()
{
    for(const auto& p : mTextures)
        glDeleteTextures(1, &p.second);
    mTextures.clear();
}

GLint TextureStore::LoadTexture(const std::string& filepath, bool alpha /*= false*/)
{
    // Check if texture isn't already loaded and if not, load it
    if(mTextures.count(filepath) == 0)
        mTextures.insert({filepath, TextureFromFile(filepath, alpha)});

    return mTextures[filepath];
}