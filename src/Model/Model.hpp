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
#include "../Movement.hpp"
#include "../Render/Shader.hpp"

template <typename Loader, typename Painter, typename MeshT>
class Model
{
public:
    /* The section below is to declare std::make_unique as a friend function.
       Doesnt seem to work on MSVC :(
    friend std::unique_ptr<Model> std::make_unique<Model>(
        const std::string&,
        const std::shared_ptr<Loader>&,
        const std::shared_ptr<Painter>&);
    */

    /// Named constructor
    static std::unique_ptr<Model> CreateModel(
        const std::string& filepath,
        const std::shared_ptr<Loader>& loader,
        const std::shared_ptr<Painter>& painter);

    /// Loads the data, first in the containers and then sends it to GPU in one go
    /// If it fails it resets the unique_ptr
    void Load(std::unique_ptr<Model>& model);

    /// Destructor
    ~Model();

    /// Use a Shader to draw meshes
    void Draw(const Shader& shader) const;

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
    Model(
        const std::string& filepath,
        const std::shared_ptr<Loader>& loader,
        const std::shared_ptr<Painter>& painter);

private:
    std::vector<GLfloat> mData;     /// Vertices, Normals, TexCoords in one vector
    std::vector<MeshT>   mMeshes;   /// Meshes for this model

    GLuint mVAO;                    /// Id for the VAO Load() used to upload data to GPU
    std::string mFilepath;          /// Filepath for this model's data

    std::shared_ptr<Loader>  mLoader;   /// Loader of this Model
    std::shared_ptr<Painter> mPainter;  /// Painter of this Model

    glm::mat4 mModelMat;

}; //~ Model

#include "Model.inl"

#endif //~ ELESWORD_MODEL_HPP
