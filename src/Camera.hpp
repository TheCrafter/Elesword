#pragma once

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// Glm
#pragma warning(push)
#pragma warning(disable:4201)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

    // Look around (camera rotation)
    GLfloat mSensitivity,
            mYaw,
            mPitch;

    /// Constructor
    Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up);

    /// Returns the view matrix for this camera object
    glm::mat4 GetView() const;

    /// Moves the camera to given direction by given distance
    void MoveCamera(MoveDirection direction, GLfloat distance);

    /// Rotates the camera based on an offset on x and y axis
    void RotateCamera(GLfloat xoffset, GLfloat yoffset);
};