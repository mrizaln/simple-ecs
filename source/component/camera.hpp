#pragma once

#include <ecs/concepts.hpp>

#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <cmath>

namespace nexus
{
    // euler angles camera
    struct Camera
    {
        static constexpr glm::vec3 world_up = { 0.0f, 1.0f, 0.0f };
        static constexpr glm::vec3 front    = { 0.0f, 0.0f, -1.0f };

        float m_fov;    // in degrees
        float m_near;
        float m_far;
        float m_speed;
        float m_sensitivity;
    };

    static_assert(ecs::concepts::Component<Camera>);
}
