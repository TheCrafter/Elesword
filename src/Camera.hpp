#pragma once

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// Glm
#pragma warning(push)
#pragma warning(disable:4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

class Camera
{
public:
    enum class MoveDirection
    {
        M_UP,
        M_DOWN,
        M_RIGHT,
        M_LEFT,
        M_IN,
        M_OUT
    };

    glm::vec3 mCameraPos,
              mCameraFront,
              mCameraUp;

    GLfloat mCameraBaseSpeed,

            // Look around (camera rotation)
            mSensitivity,
            mYaw,
            mPitch;

    Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up);

    glm::mat4 GetView() const;

    void MoveCamera(MoveDirection direction, GLfloat distance);

    void RotateCamera(GLfloat xoffset, GLfloat yoffset);

private:
    
};