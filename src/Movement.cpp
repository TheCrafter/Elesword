#include "Movement.hpp"

namespace Movement
{
    //--------------------------------------------------
    // glm::vec3 specializations
    //--------------------------------------------------
    template <>
    void Move<MoveDirection::Up, glm::vec3>(glm::vec3& pos, float distance)
    { pos.y += distance; }

    template <>
    void Move<MoveDirection::Down, glm::vec3>(glm::vec3& pos, float distance)
    { pos.y -= distance; }

    template <>
    void Move<MoveDirection::Right, glm::vec3>(glm::vec3& pos, float distance)
    { pos.x += distance; }

    template <>
    void Move<MoveDirection::Left, glm::vec3>(glm::vec3& pos, float distance)
    { pos.x -= distance; }

    template <>
    void Move<MoveDirection::In, glm::vec3>(glm::vec3& pos, float distance)
    { pos.z += distance; }

    template <>
    void Move<MoveDirection::Out, glm::vec3>(glm::vec3& pos, float distance)
    { pos.z -= distance; }

    //--------------------------------------------------
    // glm::mat4 specializations
    //--------------------------------------------------
    template <>
    void Move<MoveDirection::Up, glm::mat4>(glm::mat4& pos, float distance)
    { pos = glm::translate(pos, glm::vec3(0.0f, distance, 0.0f)); }

    template <>
    void Move<MoveDirection::Down, glm::mat4>(glm::mat4& pos, float distance)
    { pos = glm::translate(pos, glm::vec3(0.0f, -distance, 0.0f)); }

    template <>
    void Move<MoveDirection::Right, glm::mat4>(glm::mat4& pos, float distance)
    { pos = glm::translate(pos, glm::vec3(distance, 0.0f, 0.0f)); }

    template <>
    void Move<MoveDirection::Left, glm::mat4>(glm::mat4& pos, float distance)
    { pos = glm::translate(pos, glm::vec3(-distance, 0.0f, 0.0f)); }

    template <>
    void Move<MoveDirection::In, glm::mat4>(glm::mat4& pos, float distance)
    { pos = glm::translate(pos, glm::vec3(0.0f, 0.0f, distance)); }

    template <>
    void Move<MoveDirection::Out, glm::mat4>(glm::mat4& pos, float distance)
    { pos = glm::translate(pos, glm::vec3(0.0f, 0.0f, -distance)); }

} //~ namespace Movement