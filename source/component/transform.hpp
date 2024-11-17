#pragma once

#include <ecs/concepts.hpp>

#include <glm/vec3.hpp>

namespace nexus
{
    struct Transform
    {
        glm::vec3 m_position;
        glm::vec3 m_rotation;    // pitch, yaw, roll (unused for now)
        glm::vec3 m_scale;
    };

    static_assert(ecs::concepts::Component<Transform>);
}
