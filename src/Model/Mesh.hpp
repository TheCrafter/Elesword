#ifndef ELESWORD_MESH_HPP
#define ELESWORD_MESH_HPP

#include "../Util/WarnGuard.hpp"

#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
WARN_GUARD_OFF

#include "../Texture.hpp"

/// Mesh class to bundle a mesh's properties
struct Mesh
{
    GLuint ebo;                     /// EBO for this mesh
    GLuint vbo;                     /// VBO for this mesh
    std::vector<GLuint> indices;    /// Indices of this mesh
    std::vector<Texture> textures;  /// Textures of this mesh
    unsigned int dataOffset;        /// Starting position in mData

    /// Constructor
    Mesh();

}; //~ Mesh

#endif //~ ELESWORD_MESH_HPP
