#include "AssimpLoader.hpp"
#include <iostream>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <SOIL.h>

#include "Config.hpp"

//--------------------------------------------------
// Declarations
//--------------------------------------------------
std::vector<Texture> SampleLoadMaterialTextures(
    aiMaterial* mat,
    aiTextureType type,
    std::string typeName,
    std::string assetRootDir,
    std::vector<Texture>& loadedTextures);

GLint SampleTextureFromFile(const std::string& path);

//--------------------------------------------------
// AssimpLoader
//--------------------------------------------------
void AssimpLoader::LoadData(
    const std::string& filepath,
    GLuint& vao,
    std::vector<GLfloat>& vData,
    std::vector<AssimpMesh>& vMeshes,
    std::vector<GLuint>& vIndices)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filepath,
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType |
                                             aiProcess_Triangulate |
                                             aiProcess_FlipUVs);

    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Put vertices to data
    std::vector<Texture> loadedTextures;
    for(unsigned int i = 0, offset = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* curMesh = scene->mMeshes[i];

        // Create new Mesh
        AssimpMesh newMesh;

        // Update its info
        newMesh.dataOffset = offset;
        newMesh.indicesNum = 0;

        // Update offset
        offset += curMesh->mNumVertices;

        // Add Data to vData vector
        for(unsigned int j = 0; j < curMesh->mNumVertices; j++)
        {
            // Vertices
            vData.push_back(curMesh->mVertices[j].x);
            vData.push_back(curMesh->mVertices[j].y);
            vData.push_back(curMesh->mVertices[j].z);

            // Normals
            vData.push_back(curMesh->mNormals[j].x);
            vData.push_back(curMesh->mNormals[j].y);
            vData.push_back(curMesh->mNormals[j].z);

            // TexCoords
            if(curMesh->mTextureCoords)
            {
                vData.push_back(curMesh->mTextureCoords[0][j].x);
                vData.push_back(curMesh->mTextureCoords[0][j].y);
                vData.push_back(curMesh->mTextureCoords[0][j].z);
            }
            else
            {
                // TODO: Handle that case
                // vData.push_back(0);
                // vData.push_back(0);
                // vData.push_back(0);
            }
        }

        newMesh.indicesOffset = (unsigned int)vIndices.size();

        // Add Indices to vector
        for(GLuint h = 0; h < curMesh->mNumFaces; h++)
        {
            aiFace* face = &(curMesh->mFaces[h]);

            for(GLuint j = 0; j < face->mNumIndices; j++)
                vIndices.push_back(face->mIndices[j] + newMesh.dataOffset);

            newMesh.indicesNum += face->mNumIndices;
        }

        // Materials
        if(curMesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[curMesh->mMaterialIndex];

            // Diffuse maps
            std::vector<Texture> diffuseMaps = SampleLoadMaterialTextures(
                material,
                aiTextureType_DIFFUSE,
                SHADER_TEXTURE_DIFFUSE_PREFIX,
                filepath.substr(0, filepath.find_last_of('/')),
                loadedTextures);
            newMesh.textures.insert(newMesh.textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // Specular maps
            std::vector<Texture> specularMaps = SampleLoadMaterialTextures(
                material,
                aiTextureType_SPECULAR,
                SHADER_TEXTURE_SPECULAR_PREFIX,
                filepath.substr(0, filepath.find_last_of('/')),
                loadedTextures);
            newMesh.textures.insert(newMesh.textures.end(), specularMaps.begin(), specularMaps.end());
        }

        // Add new mesh to vector
        vMeshes.push_back(newMesh);
    }

    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        {
            // Bind data
            glBufferData(
                GL_ARRAY_BUFFER,
                vData.size() * sizeof(GLfloat),
                vData.data(),
                GL_STATIC_DRAW);

            // Vertices
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);

            // Normals
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);

            // TexCoords
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (GLvoid*)(6 * sizeof(GLfloat)));
            glEnableVertexAttribArray(2);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            vIndices.size() * sizeof(GLuint),
            vIndices.data(),
            GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

//--------------------------------------------------
// AssimpPainter
//--------------------------------------------------
void AssimpPainter::DrawMesh(
    const Shader& shader,
    GLuint vao,
    const std::vector<GLuint>::value_type* indices,
    const AssimpMesh& mesh) const
{
    shader.Use();

    // Bind appropriate textures
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;

    // Bind textures
    std::vector<Texture>::size_type index2 = 0;
    for(Texture texture : mesh.textures)
    {
        glActiveTexture(GL_TEXTURE0 + (GLuint)index2);

        // Retrieve texture number (the N in diffuse_textureN)
        std::string number;
        TextureType type = texture.type;

        // Transfer GLuint to stream
        switch(type)
        {
            case TextureType::DIFFUSE:
                number += (char)diffuseNr++;
                break;

            case TextureType::SPECULAR:
                number += (char)specularNr++;
                break;
            default:
                break;
        }

        // Now set the sampler to the correct texture unit
        glUniform1i(
            glGetUniformLocation(shader.GetProgID(), ("material." + TextureTypeNames[(size_t)type] + number).c_str()),
            (GLuint)index2);

        // And finally bind the texture
        glBindTexture(GL_TEXTURE_2D, texture.id);

        index2++;
    }

    // Also set each mesh's shininess property to a default value
    // (if you want you could extend this to another mesh property and possibly change this value)
    glUniform1f(glGetUniformLocation(shader.GetProgID(), "material.shininess"), 16.0f);

    // Draw mesh
    glBindVertexArray(vao);
    glDrawElements(
        GL_TRIANGLES,
        mesh.indicesNum,
        GL_UNSIGNED_INT,
        indices);
    glBindVertexArray(0);

    // "Unbind" textures
    index2 = 0;
    for(Texture texture : mesh.textures)
    {
        glActiveTexture(GL_TEXTURE0 + (GLuint)index2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

//--------------------------------------------------
// Util
//--------------------------------------------------
std::vector<Texture> SampleLoadMaterialTextures(
    aiMaterial* mat,
    aiTextureType type,
    std::string typeName,
    std::string assetRootDir,
    std::vector<Texture>& loadedTextures)
{
    std::vector<Texture> textures;

    for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);
        // Check if texture was loaded before and if so,
        // continue to next iteration: skip loading a new texture
        GLboolean skip = false;
        for(GLuint j = 0; j < loadedTextures.size(); j++)
        {
            if(loadedTextures[j].path.compare(path.C_Str()) == 0)
            {
                textures.push_back(loadedTextures[j]);
                // A texture with the same filepath has already been loaded, continue to next one
                skip = true;
                break;
            }
        }

        if(!skip)
        {   // If texture hasn't been loaded already, load it

            // Retrieve absolute filepath
            std::string absPath(assetRootDir + '/' + path.C_Str());

            Texture texture;
            texture.id = SampleTextureFromFile(absPath);

            auto it = std::find(TextureTypeNames.begin(), TextureTypeNames.end(), typeName);
            texture.type = (TextureType)std::distance(TextureTypeNames.begin(), it);

            texture.path = path.C_Str();
            textures.push_back(texture);

            // Store it as texture loaded for entire model, to ensure we won't unnecessary
            // load duplicate textures.
            loadedTextures.push_back(std::move(texture));
        }
    }

    return textures;
}

GLint SampleTextureFromFile(const std::string& path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Load image
    int width, height;
    unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    SOIL_free_image_data(image);
    return textureID;
}