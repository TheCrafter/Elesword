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

#include "Mesh.hpp"
#include "../Config.hpp"
#include "../Movement.hpp"
#include "../Render/Shader.hpp"
#include "../Texture.hpp"

class Model
{
public:
    using LoadCb = std::function<bool(
        const std::string&,     // Filepath of model
        GLuint&,                // Model's vao
        GLuint&,                // Model's vbo
        std::vector<GLfloat>&,  // Model's data
        std::vector<Mesh>&)>;   // Model's meshes

    using RenderMeshCb = std::function<void(
        const Shader& shader,
        GLuint vao,
        const Mesh& mesh)>;

    /* The section below is to declare std::make_unique as a friend function.
       Doesnt seem to work on MSVC :(
    friend std::unique_ptr<Model> std::make_unique<Model>(
        const std::string&,
        const std::shared_ptr<Loader>&,
        const std::shared_ptr<Painter>&);
    */

    /// Named constructor
    static std::unique_ptr<Model> CreateModel(const std::string& filepath, LoadCb load, RenderMeshCb renderMesh);

    /// Destructor
    ~Model();

    /// Loads the data, first in the containers and then sends it to GPU in one go
    /// If it fails it resets the unique_ptr
    void Load(std::unique_ptr<Model>& model);

    /// Use a Shader to draw meshes
    void Render(const Shader& shader) const;

    /// Resets the Model's model matrix
    void Reset();

    /// Translates the Model with a given vec3
    void Translate(const glm::vec3& tvec);
    void Translate(glm::vec3&& tvec);

    /// Scales the Model with a given vec3
    void Scale(const glm::vec3& svec);
    void Scale(glm::vec3&& svec);

    /// Moves the Model on given direction for given distance
    template <Movement::MoveDirection MD>
    void Move(float distance);

    /// Retrieves the Model's model matrix
    const glm::mat4& GetModelMat() const;

    /// Retrieves the Model's filepath
    const std::string& GetFilepath() const;

protected:
    /// Constructor
    Model(const std::string& filepath, LoadCb load, RenderMeshCb renderMesh);

private:
    std::vector<GLfloat> mData;   /// Vertices, Normals, TexCoords in one vector
    std::vector<Mesh>    mMeshes; /// Meshes for this model

    GLuint mVAO,                  /// Ids for the VAO and VOB Load() used to upload data to GPU
           mVBO;
    std::string mFilepath;        /// Filepath for this model's data

    LoadCb       mLoad;           /// Callback to a function to load this model
    RenderMeshCb mRenderMesh;     /// Callback to a function to render a mesh

    glm::mat4 mModelMat;          /// Model 4x4 matrix for this model

}; //~ Model

template <Movement::MoveDirection MD>
void Model::Move(float distance)
{
    Movement::Move<MD, glm::mat4>(mModelMat, distance);
}

#endif //~ ELESWORD_MODEL_HPP
