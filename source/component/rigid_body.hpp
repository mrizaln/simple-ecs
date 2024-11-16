#pragma once

#include <ecs/concepts.hpp>

#include <glm/vec3.hpp>

namespace nexus
{
    struct RigidBody
    {
        glm::vec3 m_velocity;
        glm::vec3 m_acceleration;
    };

    static_assert(ecs::concepts::Component<RigidBody>);
}
