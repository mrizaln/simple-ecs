#pragma once

#include <ecs/concepts.hpp>

#include <glm/vec3.hpp>

namespace nexus
{
    struct Renderable
    {
        glm::vec3 m_color;
    };

    static_assert(ecs::concepts::Component<Renderable>);
}
