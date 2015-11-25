//--------------------------------------------------
// Static functions
//--------------------------------------------------
template <typename Loader, typename Painter, typename MeshT>
std::unique_ptr<Model<Loader, Painter, MeshT>> Model<Loader, Painter, MeshT>::CreateModel(
    const std::string& filepath,
    const std::shared_ptr<Loader>& loader,
    const std::shared_ptr<Painter>& painter)
{
    return std::unique_ptr<Model<Loader, Painter, MeshT>>(new Model(filepath, loader, painter));
    //return std::make_unique<Model>(filepath, loader, painter); // Needs std::make_unique to be a friend
}

//--------------------------------------------------
// Public functions
//--------------------------------------------------
template <typename Loader, typename Painter, typename MeshT>
void Model<Loader, Painter, MeshT>::Load(std::unique_ptr<Model<Loader, Painter, MeshT>>& model)
{
    if(!model->mLoader->LoadData(mFilepath, mVAO, mData, mMeshes))
        model.reset();
}

template <typename Loader, typename Painter, typename MeshT>
Model<Loader, Painter, MeshT>::~Model()
{
    std::vector<GLuint> meshEBOs;
    for(MeshT mesh : mMeshes)
        meshEBOs.push_back(mesh.ebo);

    glDeleteBuffers((GLsizei)meshEBOs.size(), meshEBOs.data());

    glDeleteVertexArrays(1, &mVAO);
}


template <typename Loader, typename Painter, typename MeshT>
void Model<Loader, Painter, MeshT>::Draw(const Shader& shader) const
{
    // Load model matrix to GPU
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgID(), "model"), 1, GL_FALSE, glm::value_ptr(mModelMat));

    // Draw meshes
    for(const MeshT& mesh : mMeshes)
        mPainter->DrawMesh(
        shader, mVAO, mesh);
}

template <typename Loader, typename Painter, typename MeshT>
void Reset()
{
    mModelMat = glm::mat4();
}

template <typename Loader, typename Painter, typename MeshT>
void Model<Loader, Painter, MeshT>::Translate(const glm::vec3& tvec)
{
    mModelMat = glm::translate(mModelMat, tvec);
}

template <typename Loader, typename Painter, typename MeshT>
void Model<Loader, Painter, MeshT>::Translate(glm::vec3&& tvec)
{
    Translate(tvec);
}

template <typename Loader, typename Painter, typename MeshT>
void Model<Loader, Painter, MeshT>::Scale(const glm::vec3& svec)
{
    mModelMat = glm::scale(mModelMat, svec);
}

template <typename Loader, typename Painter, typename MeshT>
void Model<Loader, Painter, MeshT>::Scale(glm::vec3&& svec)
{
    Scale(svec);
}

template <typename Loader, typename Painter, typename MeshT>
template <Movement::MoveDirection MD>
void Model<Loader, Painter, MeshT>::Move(float distance)
{
    Movement::Move<MD, glm::mat4>(mModelMat, distance);
}

template <typename Loader, typename Painter, typename MeshT>
const glm::mat4& Model<Loader, Painter, MeshT>::GetModelMat()
{
    return mModelMat;
}

//--------------------------------------------------
// Private functions
//--------------------------------------------------
template <typename Loader, typename Painter, typename MeshT>
Model<Loader, Painter, MeshT>::Model(
    const std::string& filepath,
    const std::shared_ptr<Loader>& loader,
    const std::shared_ptr<Painter>& painter)
    : mFilepath(filepath)
    , mLoader(loader)
    , mPainter(painter)
{
}