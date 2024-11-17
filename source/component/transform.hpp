#pragma once

#include <ecs/concepts.hpp>

#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>

namespace nexus
{
    struct Transform
    {
        glm::vec3 m_position;
        glm::vec3 m_scale;
        glm::quat m_rotation;
    };

    static_assert(ecs::concepts::Component<Transform>);
}
