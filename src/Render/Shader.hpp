#ifndef ELESWORD_SHADER_HPP
#define ELESWORD_SHADER_HPP

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include "../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
WARN_GUARD_OFF

class Shader
{
public:
    /// Initialize the shader
    void Init(const std::string& vertexPath, const std::string& fragmentPath);

    /// Use the program
    void Use() const;

    void LoadView(const glm::mat4& view) const;
    void LoadProjection(const glm::mat4& proj) const;

    /// Retrieves the id of this shader
    GLuint GetProgID() const;

private:
    /// Program ID
    GLuint mProgramID;

}; //~ Shader

#endif //~ ELESWORD_SHADER_HPP
