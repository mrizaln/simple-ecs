#pragma once

#include <ecs/concepts.hpp>

#include <glm/vec3.hpp>

namespace nexus
{
    struct Thrust
    {
        glm::vec3 m_force;
    };

    static_assert(ecs::concepts::Component<Thrust>);
}
