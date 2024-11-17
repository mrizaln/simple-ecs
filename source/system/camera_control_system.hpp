#pragma once

#include "ecs_config.hpp"

#include <glfw_cpp/window.hpp>

namespace nexus
{
    class CameraControlSystem final : public ecs_config::ISystem
    {
    public:
        using Components = std::tuple<Camera, Transform>;

        ~CameraControlSystem() override = default;

        CameraControlSystem(glfw_cpp::Window& window);

        void update(
            ecs_config::Coordinator&     context,
            const std::set<ecs::Entity>& entities,
            ecs::Duration                frame_time
        ) override;

    private:
        glfw_cpp::Window&         m_window;
        std::pair<double, double> m_cursor_pos;
    };
}
