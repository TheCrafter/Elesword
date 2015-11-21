#include "Camera.hpp"

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
{
    mCameraPos   = pos;
    mCameraFront = front;
    mCameraUp    = up;

    mSensitivity     =  0.05f;
    mYaw             = -90.0f; // We need that inverted initially
    mPitch           =  0.0f;
}

glm::mat4 Camera::GetView() const
{
    return glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
}

void Camera::MoveCamera(Movement::MoveDirection direction, GLfloat distance)
{
    switch(direction)
    {
        case Movement::MoveDirection::In:
            mCameraPos += distance * mCameraFront;
            break;

        case Movement::MoveDirection::Out:
            mCameraPos -= distance * mCameraFront;
            break;

        case Movement::MoveDirection::Left:
            mCameraPos -= glm::normalize(glm::cross(mCameraFront, mCameraUp)) * distance;
            break;

        case Movement::MoveDirection::Right:
            mCameraPos += glm::normalize(glm::cross(mCameraFront, mCameraUp)) * distance;
            break;
    }
}

void Camera::RotateCamera(GLfloat xoffset, GLfloat yoffset)
{
    xoffset *= mSensitivity;
    yoffset *= mSensitivity;

    mYaw += xoffset;
    mPitch += yoffset;

    if(mPitch > 89.0f)
        mPitch = 89.0f;
    if(mPitch < -89.0f)
        mPitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    front.y = sin(glm::radians(mPitch));
    front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mCameraFront = glm::normalize(front);
}