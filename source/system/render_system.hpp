#pragma once

#include "component/renderable.hpp"
#include "component/transform.hpp"
#include "ecs_config.hpp"
#include "graphics/cube_primitive.hpp"
#include "graphics/shader.hpp"

#include <glfw_cpp/window.hpp>

namespace nexus
{
    class RenderSystem final : public ecs_config::ISystem
    {
    public:
        using Components = std::tuple<Renderable, Transform>;

        ~RenderSystem() override = default;

        // render_context will be stored inside the class, but the coordinator will not
        RenderSystem(ecs_config::Coordinator& coordinator, glfw_cpp::Window& render_context, Shader shader);

        void update(
            ecs_config::Coordinator&     context,
            const std::set<ecs::Entity>& entities,
            ecs::Duration                frame_time
        ) override;

    private:
        void draw();

        glfw_cpp::Window& m_render_context;
        ecs::Entity       m_camera;
        CubePrimitive     m_cube;
        Shader            m_shader;
    };
}
