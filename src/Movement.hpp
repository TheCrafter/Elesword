#ifndef ELESWORD_MOVEMENT_HPP
#define ELESWORD_MOVEMENT_HPP

#include "Util/WarnGuard.hpp"

WARN_GUARD_ON
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
WARN_GUARD_OFF

namespace Movement
{
    enum class MoveDirection
    {
        Up,
        Down,
        Right,
        Left,
        In,
        Out
    };

    template <MoveDirection MD, typename PosType>
    void Move(PosType& pos, float distance);

} //~ namespace Movement

#endif //~ ELESWORD_MOVEMENT_HPP
