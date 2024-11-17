#include "render_system.hpp"
#include "component/camera.hpp"

#include <ecs/coordinator.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glbinding/gl/gl.h>

namespace
{
    // euler angle rotation
    glm::mat4 rotation_matrix(glm::mat4 mat, const glm::vec3& rotation)
    {
        auto pitch = rotation.x;
        auto yaw   = rotation.y;
        auto roll  = rotation.z;

        mat = glm::rotate(mat, glm::radians(pitch), { 1.0f, 0.0f, 0.0f });
        mat = glm::rotate(mat, glm::radians(yaw), { 0.0f, 1.0f, 0.0f });
        mat = glm::rotate(mat, glm::radians(roll), { 0.0f, 0.0f, 1.0f });

        return mat;
    }
}

namespace nexus
{
    RenderSystem::RenderSystem(
        ecs_config::Coordinator& coordinator,
        glfw_cpp::Window&        render_context,
        Shader                   shader
    )
        : m_render_context{ render_context }
        , m_camera{ coordinator.create_entity() }
        , m_cube{ 1.0f }
        , m_shader{ std::move(shader) }
    {
        coordinator.add_component(
            m_camera,
            Transform{
                .m_position = { 0.0f, 0.0f, 3.0f },
                .m_rotation = { 0.0f, 270.0f, 0.0f },
                .m_scale    = glm::vec3{ 1.0f },
            }
        );

        const auto& rotation = coordinator.get_component<Transform>(m_camera).m_rotation;
        coordinator.add_component(m_camera, Camera::create(rotation, 90.0f, 0.1f, 100.0f));

        gl::glClearColor(0.1f, 0.1f, 0.11f, 1.0f);
    }

    void RenderSystem::update(
        ecs_config::Coordinator&     context,
        const std::set<ecs::Entity>& entities,
        ecs::Duration /* frame_time */
    )
    {
        m_render_context.bind();

        auto&& [width, height] = m_render_context.properties().m_dimension;

        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
        gl::glViewport(0, 0, width, height);

        m_shader.use();

        auto camera           = context.get_component<Camera>(m_camera);
        auto camera_transform = context.get_component<Transform>(m_camera);

        auto view       = camera.view_matrix(camera_transform.m_position);
        auto projection = camera.projection_matrix((float)width, (float)height);

        m_shader.set_uniform("u_view", view);
        m_shader.set_uniform("u_projection", projection);

        for (const auto& entity : entities) {
            auto&& [renderable, transform] = context.get_component_tuple<Components>(entity);

            auto model = glm::translate(glm::mat4{ 1.0f }, transform.m_position);
            model      = glm::scale(model, transform.m_scale);
            model      = rotation_matrix(model, transform.m_rotation);

            m_shader.set_uniform("u_model", model);
            m_shader.set_uniform("u_color", renderable.m_color);

            m_cube.draw();
        }

        m_shader.unuse();

        m_render_context.display();
        m_render_context.unbind();
    }
}