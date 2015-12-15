#include "Model.hpp"

//--------------------------------------------------
// ModelData functions
//--------------------------------------------------
ModelData::~ModelData()
{
    std::vector<GLuint> meshEBOs;
    for(Mesh mesh : meshes)
        meshEBOs.push_back(mesh.ebo);

    glDeleteBuffers((GLsizei)meshEBOs.size(), meshEBOs.data());
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

//--------------------------------------------------
// Static functions
//--------------------------------------------------
std::unique_ptr<Model> Model::CreateModel(
    const ModelData* const data,
    RenderMeshCb renderMesh)
{
    return (data == nullptr) ? nullptr : std::unique_ptr<Model>(new Model(data, renderMesh));
    //return std::make_unique<Model>(filepath, loader, painter); // Needs std::make_unique to be a friend
}

//--------------------------------------------------
// Public functions
//--------------------------------------------------
void Model::Render(const Shader& shader) const
{
    shader.Use();

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    // Load model matrix to GPU
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgID(), "model"), 1, GL_FALSE, glm::value_ptr(mModelMat));

    // Draw meshes
    for(const Mesh& mesh : mData->meshes)
        mRenderMesh(shader, mData->vao, mesh);
}

void Model::RenderOutline(const Shader& shader) const
{
    shader.Use();

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    // Copy model's model matrix
    glm::mat4 outlineModelMat = mModelMat;

    // Scale it up and move it down to look like outline
    outlineModelMat = glm::translate(outlineModelMat, glm::vec3(0.0f, -0.2f, 0.0f));
    outlineModelMat = glm::scale(outlineModelMat, glm::vec3(1.03f, 1.03f, 1.03f));

    // Load model matrix to GPU
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgID(), "model"), 1, GL_FALSE, glm::value_ptr(outlineModelMat));

    // Draw meshes
    for(const Mesh& mesh : mData->meshes)
        mRenderMesh(shader, mData->vao, mesh);

    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
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

//--------------------------------------------------
// Private functions
//--------------------------------------------------
Model::Model(const ModelData* const data, RenderMeshCb renderMesh)
    : mData(data)
    , mRenderMesh(renderMesh)
{
}