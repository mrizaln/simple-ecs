#pragma once

#include <ecs/concepts.hpp>

#include <glm/mat4x4.hpp>

namespace nexus
{
    struct Camera
    {
        glm::mat4 m_projection_transform;
    };

    static_assert(ecs::concepts::Component<Camera>);
}
