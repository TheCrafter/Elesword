#include "AssimpLoader.hpp"
#include <iostream>
#include <SOIL.h>

// Forward declarations


GLint TextureFromFile(const std::string& path);

void AssimpLoader::AssimpDraw(const Shader& shader, const std::vector<AssimpMesh>* meshes)
{
    shader.Use();

    for(std::vector<AssimpMesh>::size_type i = 0; i < meshes->size(); i++)
    {
        // Bind appropriate textures
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;

        // Bind textures
        for(std::vector<AssimpTexture>::size_type j = 0; j != (*meshes)[i].textures.size(); j++)
        {
            glActiveTexture(GL_TEXTURE0 + (GLuint)j);

            // Retrieve texture number (the N in diffuse_textureN)
            std::string number;
            AssimpTextureType type = (*meshes)[i].textures[j].type;

            // Transfer GLuint to stream
            switch(type)
            {
                case AssimpTextureType::DIFFUSE:
                    number += (char)diffuseNr++;
                    break;

                case AssimpTextureType::SPECULAR:
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
            glBindTexture(GL_TEXTURE_2D, (*meshes)[i].textures[j].id);
        }

        // Also set each mesh's shininess property to a default value
        // (if you want you could extend this to another mesh property and possibly change this value)
        glUniform1f(glGetUniformLocation(shader.GetProgID(), "material.shininess"), 16.0f);

        // Draw mesh
        glBindVertexArray((*meshes)[i].VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)(*meshes)[i].indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // "Unbind" textures
        for(std::vector<AssimpTexture>::size_type j = 0; j != (*meshes)[j].textures.size(); j++)
        {
            glActiveTexture(GL_TEXTURE0 + (GLuint)j);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

std::vector<AssimpLoader::AssimpMesh>* AssimpLoader::AssimpLoad(const std::string& filepath)
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
        return nullptr;
    }

    return LoadToGpu(scene, filepath);
}

std::vector<AssimpLoader::AssimpMesh>* AssimpLoader::LoadToGpu(const aiScene* scene, const std::string& filepath)
{
    std::vector<AssimpMesh>* meshVecPtr = new std::vector<AssimpMesh>();
    std::vector<AssimpTexture> loadedTextures;

    for(unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        AssimpMesh newMesh;

        aiMesh* mesh = scene->mMeshes[i];

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
            std::vector<AssimpTexture> diffuseMaps = LoadMaterialTextures(
                material,
                aiTextureType_DIFFUSE,
                SHADER_TEXTURE_DIFFUSE_PREFIX,
                filepath.substr(0, filepath.find_last_of('/')),
                loadedTextures);
            newMesh.textures.insert(newMesh.textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // newMesh.textures.push_back(LoadMaterialTextures(
            //     material,
            //     aiTextureType_DIFFUSE,
            //     SHADER_TEXTURE_DIFFUSE_PREFIX,
            //     mFilepath.substr(0, mFilepath.find_last_of('/')),
            //     loadedTextures,
            //     mLoadTextureFromFileCb));

            // Specular maps
            std::vector<AssimpTexture> specularMaps = LoadMaterialTextures(
                material,
                aiTextureType_SPECULAR,
                SHADER_TEXTURE_SPECULAR_PREFIX,
                filepath.substr(0, filepath.find_last_of('/')),
                loadedTextures);

            newMesh.textures.insert(newMesh.textures.end(), specularMaps.begin(), specularMaps.end());
            //newMesh.textures.push_back();
        }

        meshVecPtr->push_back(std::move(newMesh));
    }

    return meshVecPtr;
}

std::vector<AssimpTexture> AssimpLoader::LoadMaterialTextures(
    aiMaterial* mat,
    aiTextureType type,
    std::string typeName,
    std::string assetRootDir,
    std::vector<AssimpTexture> loadedTextures)
{
    std::vector<AssimpTexture> textures;

    for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);
        // Check if texture was loaded before and if so,
        // continue to next iteration: skip loading a new texture
        GLboolean skip = false;
        for(GLuint j = 0; j < loadedTextures.size(); j++)
        {
            if(loadedTextures[j].path == path)
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

            AssimpTexture texture;
            texture.id = TextureFromFile(absPath);

            auto it = std::find(TextureTypeNames.begin(), TextureTypeNames.end(), typeName);
            texture.type = (AssimpTextureType)std::distance(TextureTypeNames.begin(), it);

            texture.path = path;
            textures.push_back(texture);

            // Store it as texture loaded for entire model, to ensure we won't unnecessary
            // load duplicate textures.
            loadedTextures.push_back(std::move(texture));
        }
    }

    return textures;
}

GLint TextureFromFile(const std::string& path)
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