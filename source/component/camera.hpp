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

        glm::vec3 m_up;
        glm::vec3 m_front;
        glm::vec3 m_right;
        float     m_fov;        // in degrees
        float     m_near;
        float     m_far;

        static Camera create(const glm::vec3& rotation, float fov, float near, float far)
        {
            auto pitch = rotation.x;
            auto yaw   = rotation.y;

            auto direction = glm::vec3{
                std::cos(pitch) * std::cos(yaw),
                std::sin(pitch),
                std::cos(pitch) * std::sin(yaw),
            };

            auto front = glm::normalize(direction);
            auto right = glm::normalize(glm::cross(front, world_up));
            auto up    = glm::normalize(glm::cross(right, front));

            return Camera{
                .m_up    = up,
                .m_front = front,
                .m_right = right,
                .m_fov   = fov,
                .m_near  = near,
                .m_far   = far,
            };
        }

        void update_camera_vector(glm::vec3 rotation)
        {
            auto pitch = rotation.x;
            auto yaw   = rotation.y;

            auto direction = glm::vec3{
                std::cos(pitch) * std::cos(yaw),
                std::sin(pitch),
                std::cos(pitch) * std::sin(yaw),
            };

            m_front = glm::normalize(direction);
            m_right = glm::normalize(glm::cross(m_front, world_up));
            m_up    = glm::normalize(glm::cross(m_right, m_front));
        }

        glm::mat4 view_matrix(glm::vec3 position) const
        {
            return glm::lookAt(position, position + m_front, m_up);
        }

        glm::mat4 projection_matrix(float width, float height) const
        {
            return glm::perspective(glm::radians(m_fov), width / height, m_near, m_far);
        }
    };

    static_assert(ecs::concepts::Component<Camera>);
}
