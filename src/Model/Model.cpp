#include "Model.hpp"

//--------------------------------------------------
// Static functions
//--------------------------------------------------
std::unique_ptr<Model> Model::CreateModel(
    const std::string& filepath,
    LoadCb load,
    RenderMeshCb renderMesh)
{
    return std::unique_ptr<Model>(new Model(filepath, load, renderMesh));
    //return std::make_unique<Model>(filepath, loader, painter); // Needs std::make_unique to be a friend
}

//--------------------------------------------------
// Public functions
//--------------------------------------------------
Model::~Model()
{
    std::vector<GLuint> meshEBOs;
    for(Mesh mesh : mMeshes)
        meshEBOs.push_back(mesh.ebo);

    glDeleteBuffers((GLsizei)meshEBOs.size(), meshEBOs.data());
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}

void Model::Load(std::unique_ptr<Model>& model)
{
    if(!mLoad(mFilepath, mVAO, mVBO, mData, mMeshes))
        model.reset();
}

void Model::Render(const Shader& shader) const
{
    // Load model matrix to GPU
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgID(), "model"), 1, GL_FALSE, glm::value_ptr(mModelMat));

    // Draw meshes
    for(const Mesh& mesh : mMeshes)
        mRenderMesh(shader, mVAO, mesh);
}

void Model::Reset()
{
    mModelMat = glm::mat4();
}

void Model::Translate(const glm::vec3& tvec)
{
    mModelMat = glm::translate(mModelMat, tvec);
}

void Model::Translate(glm::vec3&& tvec)
{
    Translate(tvec);
}

void Model::Scale(const glm::vec3& svec)
{
    mModelMat = glm::scale(mModelMat, svec);
}

void Model::Scale(glm::vec3&& svec)
{
    Scale(svec);
}

const glm::mat4& Model::GetModelMat() const
{
    return mModelMat;
}

const std::string& Model::GetFilepath() const
{
    return mFilepath;
}

//--------------------------------------------------
// Private functions
//--------------------------------------------------
Model::Model(const std::string& filepath, LoadCb load, RenderMeshCb renderMesh)
    : mFilepath(filepath)
    , mLoad(load)
    , mRenderMesh(renderMesh)
{
}