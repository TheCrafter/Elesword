#include "Light.hpp"

template<>
void LoadLightSpecific<PointLight>(
    GLuint shaderId,
    const PointLight& light,
    const std::string& glslUniformName)
{
    glUniform3f(glGetUniformLocation(shaderId, (glslUniformName + ".position").c_str()),  light.attr.position.x, light.attr.position.y, light.attr.position.z);
    glUniform1f(glGetUniformLocation(shaderId, (glslUniformName + ".constant").c_str()),  light.attr.constant);
    glUniform1f(glGetUniformLocation(shaderId, (glslUniformName + ".linear").c_str()),    light.attr.linear);
    glUniform1f(glGetUniformLocation(shaderId, (glslUniformName + ".quadratic").c_str()), light.attr.quadratic);
}

template<>
void LoadLightSpecific<DirLight>(
    GLuint shaderId,
    const DirLight& light,
    const std::string& glslUniformName)
{
    glUniform3f(glGetUniformLocation(shaderId, (glslUniformName + ".direction").c_str()), light.attr.direction.x, light.attr.direction.y, light.attr.direction.z);
}

template<>
void LoadLightSpecific<SpotLight>(
    GLuint shaderId,
    const SpotLight& light,
    const std::string& glslUniformName)
{
    glUniform3f(glGetUniformLocation(shaderId, (glslUniformName + ".position").c_str()),  light.attr.position.x,  light.attr.position.y,  light.attr.position.z);
    glUniform3f(glGetUniformLocation(shaderId, (glslUniformName + ".direction").c_str()), light.attr.direction.x, light.attr.direction.y, light.attr.direction.z);
}