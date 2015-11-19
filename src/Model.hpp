#ifndef ELESWORD_MODEL_HPP
#define ELESWORD_MODEL_HPP

#include "WarnGuard.hpp"

#include <string>
#include <vector>
#include <functional>
#include <memory>

WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
WARN_GUARD_OFF

#include "Config.hpp"
#include "Shader.hpp"

template <typename MeshT>
class Model
{
public:
    using DrawCb = std::function<void(const Shader& shader, const std::vector<MeshT>* meshes)>;
    using LoadMeshesCb = std::function<std::vector<MeshT>*(const std::string& filepath)>;

    // Constructor
    Model(const std::string& filepath, DrawCb dcb, LoadMeshesCb lmcb)
        : mFilepath(filepath)
        , mDrawCb(dcb)
        , mMeshes(lmcb(filepath))
    {
    }

    // Use Draw Cb to draw meshes
    void Draw(const Shader& shader) { mDrawCb(shader, mMeshes.get()); }

    // Getters
    const std::vector<MeshT>* GetMeshes() const { return mMeshes.get(); }

private:
    std::unique_ptr<std::vector<MeshT>> mMeshes;
    std::string mFilepath;

    DrawCb mDrawCb;

}; //~ Model

#endif //~ ELESWORD_MODEL_HPP
