#include "Model.hpp"

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

const std::array<std::string, sizeof(Model::TextureType)> Model::TextureTypeNames = {{
        SHADER_TEXTURE_DIFFUSE_PREFIX,
        SHADER_TEXTURE_SPECULAR_PREFIX}};

//--------------------------------------------------
// Public functions
//--------------------------------------------------

Model::Model(const std::string& filepath, LoadTextureCb cb)
    : mFilepath(filepath)
    , mLoadTextureFromFileCb(cb)
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

    LoadToGpu(scene);
}

void Model::Draw(const Shader& shader)
{
    shader.Use();

    for(std::vector<Mesh>::size_type i = 0; i < mMeshes.size(); i++)
    {
        // Bind appropriate textures
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;

        // Bind textures
        for(std::vector<Texture>::size_type j = 0; j != mMeshes[i].textures.size(); j++)
        {
            glActiveTexture(GL_TEXTURE0 + (GLuint)j);

            // Retrieve texture number (the N in diffuse_textureN)
            std::string number;
            TextureType type = mMeshes[i].textures[j].type;

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
                (GLuint)j);

            // And finally bind the texture
            glBindTexture(GL_TEXTURE_2D, mMeshes[i].textures[j].id);
        }

        // Also set each mesh's shininess property to a default value
        // (if you want you could extend this to another mesh property and possibly change this value)
        glUniform1f(glGetUniformLocation(shader.GetProgID(), "material.shininess"), 16.0f);

        // Draw mesh
        glBindVertexArray(mMeshes[i].VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)mMeshes[i].indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // "Unbind" textures
        for(std::vector<Texture>::size_type j = 0; j != mMeshes[j].textures.size(); j++)
        {
            glActiveTexture(GL_TEXTURE0 + (GLuint)j);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

void Model::SetLoadTextureFromFileCb(LoadTextureCb cb)
{
    mLoadTextureFromFileCb = cb;
}

//--------------------------------------------------
// Private functions
//--------------------------------------------------

void Model::LoadToGpu(const aiScene* scene)
{
    for(unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        Mesh newMesh;

        aiMesh* mesh = scene->mMeshes[i];
        mesh->mVertices;

        std::array<GLuint, 4> bufObjAr;
        glGenBuffers((GLsizei)bufObjAr.size(), bufObjAr.data());
        GLuint VBO = bufObjAr[0], // Vertices
            NBO = bufObjAr[1], // Normals
            TBO = bufObjAr[2], // Textures
            EBO = bufObjAr[3]; // Elements

        glGenVertexArrays(1, &(newMesh.VAO));

        glBindVertexArray(newMesh.VAO);
        {
            // Vertices
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            {
                glBufferData(
                    GL_ARRAY_BUFFER,
                    mesh->mNumVertices * sizeof(aiVector3D),
                    mesh->mVertices,
                    GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
                glEnableVertexAttribArray(0);
            }

            // Normals
            glBindBuffer(GL_ARRAY_BUFFER, NBO);
            {
                glBufferData(
                    GL_ARRAY_BUFFER,
                    mesh->mNumVertices * sizeof(aiVector3D),
                    mesh->mNormals,
                    GL_STATIC_DRAW);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
                glEnableVertexAttribArray(1);
            }

            // Texture Coordinates
            if(mesh->mTextureCoords)
            {
                glBindBuffer(GL_ARRAY_BUFFER, TBO);
                {
                    glBufferData(
                        GL_ARRAY_BUFFER,
                        mesh->mNumVertices * sizeof(aiVector3D),
                        mesh->mTextureCoords[0],
                        GL_STATIC_DRAW);
                    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
                    glEnableVertexAttribArray(2);
                }
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            // Indices
            for(GLuint h = 0; h < mesh->mNumFaces; h++)
            {
                aiFace* face = &(mesh->mFaces[h]);
                for(GLuint j = 0; j < face->mNumIndices; j++)
                    newMesh.indices.push_back(face->mIndices[j]);
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, newMesh.indices.size() * sizeof(GLuint), newMesh.indices.data(), GL_STATIC_DRAW);;
        }
        glBindVertexArray(0);

        // Materials
        if(mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // Diffuse maps
            std::vector<Texture> diffuseMaps = LoadMaterialTextures(
                material,
                aiTextureType_DIFFUSE,
                SHADER_TEXTURE_DIFFUSE_PREFIX);
            newMesh.textures.insert(newMesh.textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // Specular maps
            std::vector<Texture> specularMaps = LoadMaterialTextures(
                material,
                aiTextureType_SPECULAR,
                SHADER_TEXTURE_SPECULAR_PREFIX);

            newMesh.textures.insert(newMesh.textures.end(), specularMaps.begin(), specularMaps.end());
        }

        mMeshes.push_back(std::move(newMesh));
    }
}

//--------------------------------------------------
// Util functions
//--------------------------------------------------

std::vector<Model::Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;

    for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);
        // Check if texture was loaded before and if so,
        // continue to next iteration: skip loading a new texture
        GLboolean skip = false;
        for(GLuint j = 0; j < mLoadedTextures.size(); j++)
        {
            if(mLoadedTextures[j].path == path)
            {
                textures.push_back(mLoadedTextures[j]);
                // A texture with the same filepath has already been loaded, continue to next one
                skip = true;
                break;
            }
        }

        if(!skip)
        {   // If texture hasn't been loaded already, load it

            // Retrieve absolute filepath
            std::string absPath(mFilepath.substr(0, mFilepath.find_last_of('/')) + '/' + path.C_Str());

            Texture texture;
            texture.id = mLoadTextureFromFileCb(absPath);

            auto it = std::find(TextureTypeNames.begin(), TextureTypeNames.end(), typeName);
            texture.type = (TextureType)std::distance(TextureTypeNames.begin(), it);

            texture.path = path;
            textures.push_back(texture);

            // Store it as texture loaded for entire model, to ensure we won't unnecessary
            // load duplicate textures.
            this->mLoadedTextures.push_back(std::move(texture));
        }
    }

    return textures;
}