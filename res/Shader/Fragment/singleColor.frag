#version 330 core
struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};
/* Note: because we now use a material struct again you want to change your
mesh class to bind all the textures using material.texture_diffuseN instead of
texture_diffuseN. */

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 2

in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
}