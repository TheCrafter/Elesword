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
#include "../Texture/Texture.hpp"

struct ModelData
{
    std::vector<GLfloat> data;    /// Vertices, Normals, TexCoords in one vector
    std::vector<Mesh>    meshes;  /// Meshes for this model

    GLuint vao,                   /// Ids for the VAO and VOB Load() used to upload data to GPU
           vbo;

    /// Destructor
    ~ModelData();

}; //~ ModelData

class Model
{
public:
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
    static std::unique_ptr<Model> CreateModel(const ModelData* const data, RenderMeshCb renderMesh);

    /// Use a Shader to draw meshes
    void Render(const Shader& shader) const;

    /// Use a Shader to draw model's outline
    void RenderOutline(const Shader& shader) const;

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

protected:
    /// Constructor
    Model(const ModelData* const mData, RenderMeshCb renderMesh);

private:
    const ModelData* mData;       /// Data for this model

    RenderMeshCb mRenderMesh;     /// Callback to a function to render a mesh

    glm::mat4 mModelMat;          /// Model 4x4 matrix for this model

}; //~ Model

template <Movement::MoveDirection MD>
void Model::Move(float distance)
{
    Movement::Move<MD, glm::mat4>(mModelMat, distance);
}

#endif //~ ELESWORD_MODEL_HPP
