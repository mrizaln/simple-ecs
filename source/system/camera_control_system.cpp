#include "camera_control_system.hpp"

#include <ecs/coordinator.hpp>
#include <glm/gtc/quaternion.hpp>

namespace nexus
{
    CameraControlSystem::CameraControlSystem(glfw_cpp::Window& window)
        : m_window{ window }
    {
    }

    void CameraControlSystem::update(
        ecs_config::Coordinator&     context,
        const std::set<ecs::Entity>& entities,
        ecs::Duration                frame_time
    )
    {
        const auto& keys   = m_window.properties().m_keyState;
        const auto& cursor = m_window.properties().m_cursor;
        auto        width  = m_window.properties().m_dimension.m_width;

        auto last_x = m_cursor_pos.first;
        auto last_y = m_cursor_pos.second;

        // scale the rate of change to the window's width
        auto cursor_dx = -(cursor.m_x - last_x) / static_cast<double>(width);
        auto cursor_dy = -(cursor.m_y - last_y) / static_cast<double>(width);

        m_cursor_pos = { cursor.m_x, cursor.m_y };

        for (const auto& entity : entities) {
            auto&& [camera, transform] = context.get_component_tuple<Components>(entity);

            // look around
            // -----------

            // FIXME: not working as inteded... I need to read the actual math of quaternion...

            // if (m_window.isMouseCaptured()) {
            //     auto sensitivity = camera.m_sensitivity;

            //     auto yaw   = cursor_dx * sensitivity;
            //     auto pitch = cursor_dy * sensitivity;

            //     yaw   = std::fmod(yaw, glm::two_pi<double>());
            //     pitch = std::clamp(pitch, -glm::half_pi<double>() * 0.99, glm::half_pi<double>() * 0.99);

            //     auto rotation        = glm::quat{ glm::vec3{ pitch, yaw, 0.0f } };
            //     transform.m_rotation = glm::normalize(rotation * transform.m_rotation);
            // }

            // -----------

            // translation
            // -----------
            auto dt    = frame_time.count();
            auto speed = camera.m_speed;
            auto dist  = speed * dt;

            auto displacement = glm::vec3{ 0.0f };

            using Key = glfw_cpp::KeyCode;

            // minecraft-like camera controls
            if (keys.isPressed(Key::W)) {
                displacement.z -= dist;
            } else if (keys.isPressed(Key::S)) {
                displacement.z += dist;
            }

            if (keys.isPressed(Key::A)) {
                displacement.x -= dist;
            } else if (keys.isPressed(Key::D)) {
                displacement.x += dist;
            }

            if (keys.isPressed(Key::Space)) {
                displacement.y += dist;
            } else if (keys.isPressed(Key::LeftShift)) {
                displacement.y -= dist;
            }

            // rotate the displacement vector by the camera's rotation
            transform.m_position += glm::vec3{ transform.m_rotation * displacement };
            // -----------
        }
    }
}
