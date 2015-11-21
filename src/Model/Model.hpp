#ifndef ELESWORD_MODEL_HPP
#define ELESWORD_MODEL_HPP

#include "../Util/WarnGuard.hpp"

#include <string>
#include <vector>
#include <functional>
#include <memory>

WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
WARN_GUARD_OFF

#include "../Config.hpp"
#include "../Shader.hpp"

template <typename Loader, typename Painter, typename MeshT>
class Model
{
public:
    // Constructor
    Model(
        const std::string& filepath,
        const std::shared_ptr<Loader>& loader,
        const std::shared_ptr<Painter>& painter)
        : mFilepath(filepath)
        , mLoader(loader)
        , mPainter(painter)
    {
    }

    // Destructor
    ~Model() { glDeleteVertexArrays(1, &mVAO); }

    // Loads the data, first in the containers and then sends it to GPU in one go
    void Load() { mLoader->LoadData(mFilepath, mVAO, mData, mMeshes, mIndices); }

    // Use a Shader to draw meshes
    void Draw(const Shader& shader) const
    {
        for(const MeshT& mesh : mMeshes)
            mPainter->DrawMesh(
                shader, mVAO, mIndices.data() + mesh.indicesOffset, mesh);
    }

    // Model operations
    void Reset() { mModelMat = glm::mat4(); }

    void Translate(glm::vec3& tvec) { mModelMat = glm::translate(mModelMat, tvec); }
    void Translate(glm::vec3&& tvec) { Translate(tvec); }

    void Scale(glm::vec3& svec) { mModelMat = glm::scale(mModelMat, svec); }
    void Scale(glm::vec3&& svec) { Scale(svec); }

    // Getters
    const glm::mat4& GetModelMat() { return mModelMat; }

private:
    std::vector<GLfloat> mData;     // Vertices, Normals, TexCoords in one vector
    std::vector<MeshT>   mMeshes;   // Meshes for this model
    std::vector<GLuint>  mIndices;  // Indicies for this model (for every mesh)

    GLuint mVAO;                    // Id for the VAO Load() used to upload data to GPU
    std::string mFilepath;          // Filepath for this model's data

    std::shared_ptr<Loader>  mLoader;
    std::shared_ptr<Painter> mPainter;

    glm::mat4 mModelMat;

}; //~ Model

#endif //~ ELESWORD_MODEL_HPP
