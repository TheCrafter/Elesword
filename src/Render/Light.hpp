#ifndef ELESWORD_LIGHT_HPP
#define ELESWORD_LIGHT_HPP

#include <string>

#include "../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

#define GLEW_STATIC
#include <GL/glew.h>

struct PointLightAttributes
{
    // Light's position
    glm::vec3 position;

    // Data for light calculations
    float constant,
          linear,
          quadratic;
};

struct DirectionalLightAttributes
{
    // Light's direction
    glm::vec3 direction;
};

struct SpotLightAttributes
{
    // Light's position
    glm::vec3 position;

    // Light's direction
    glm::vec3 direction;
};

template <typename LightAttributes>
struct Light
{
    // Attributes
    LightAttributes attr;

    // Color
    glm::vec3 ambient,
              diffuse,
              specular;
};

using PointLight = Light<PointLightAttributes>;
using DirLight   = Light<DirectionalLightAttributes>;
using SpotLight  = Light<SpotLightAttributes>;

template <typename LightType>
void LoadLight(GLuint shaderId, const LightType& light, const std::string& glslUniformName)
{
    // Load common light attributes
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), (glslUniformName + ".ambient").c_str()),  light.ambient.x,  light.ambient.y,  light.ambient.z);
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), (glslUniformName + ".diffuse").c_str()),  light.diffuse.x,  light.diffuse.y,  light.diffuse.z);
    glUniform3f(glGetUniformLocation(lightingShader.GetProgID(), (glslUniformName + ".specular").c_str()), light.specular.x, light.specular.y, light.specular.z);

    // Load specific light attributes
    LoadLightSpecific<LightType>(shaderId, light, glslUniformName);
}

// TODO: See if that works in GCC/LLVM
// I declared LoadLightSpecific here but I use it in LoadLight. In MSVC everything's fine
// but in GCC/LLVM it **should** not work. I need to find out before fixing it
template <typename LightType>
void LoadLightSpecific(GLuint shaderId, const LightType& light, const std::string& glslUniformName);

#endif //~ ELESWORD_LIGHT_HPP