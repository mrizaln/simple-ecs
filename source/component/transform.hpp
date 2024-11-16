#pragma once

#include <ecs/concepts.hpp>

#include <glm/vec3.hpp>

namespace nexus
{
    struct Transform
    {
        glm::vec3 m_force;
    };

    static_assert(ecs::concepts::Component<Transform>);
}
