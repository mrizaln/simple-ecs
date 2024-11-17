#include "render_system.hpp"

#include "component/camera.hpp"

#include <ecs/coordinator.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glbinding/gl/gl.h>

#include <print>

namespace
{
    glm::mat4 view_matrix(glm::vec3 position, glm::quat rotation)
    {
        auto front = rotation * nexus::Camera::front;
        return glm::lookAt(position, position + front, nexus::Camera::world_up);
    }

    glm::mat4 projection_matrix(float width, float height, float fov, float near, float far)
    {
        return glm::perspective(glm::radians(fov), width / height, near, far);
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
                .m_position = { 0.0f, -50.0f, 200.0f },
                .m_scale    = glm::vec3{ 1.0f },
                .m_rotation = glm::vec3{ 0.0f, 0.0f, 0.0f },
            }
        );
        coordinator.add_component(m_camera, Camera{ 90.0f, 0.1f, 1000.0f, 20.0f, 1.0f });

        gl::glClearColor(0.1f, 0.1f, 0.11f, 1.0f);
        gl::glEnable(gl::GL_DEPTH_TEST);
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

        const auto& [fov, near, far, speed, sensitivity] = context.get_component<Camera>(m_camera);
        const auto& [cam_pos, cam_scale, cam_rot]        = context.get_component<Transform>(m_camera);

        auto view       = view_matrix(cam_pos, cam_rot);
        auto projection = projection_matrix((float)width, (float)height, fov, near, far);

        m_shader.set_uniform("u_view", view);
        m_shader.set_uniform("u_projection", projection);

        for (const auto& entity : entities) {
            auto&& [renderable, transform] = context.get_component_tuple<Components>(entity);

            auto model = glm::translate(glm::mat4{ 1.0f }, transform.m_position);
            model      = glm::scale(model, transform.m_scale);
            model      = model * glm::mat4_cast(transform.m_rotation);

            m_shader.set_uniform("u_model", model);
            m_shader.set_uniform("u_color", renderable.m_color);

            m_cube.draw();
        }

        m_shader.unuse();

        m_render_context.display();
        m_render_context.unbind();
    }
}
