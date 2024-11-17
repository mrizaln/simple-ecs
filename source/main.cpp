#include "component/renderable.hpp"
#include "ecs/util/concepts.hpp"
#include "ecs_config.hpp"
#include "component/gravity.hpp"
#include "component/rigid_body.hpp"
#include "component/transform.hpp"
#include "graphics/shader.hpp"
#include "system/physics_system.hpp"
#include "system/render_system.hpp"

#include <ecs/common.hpp>
#include <ecs/entity_manager.hpp>
#include <ecs/coordinator.hpp>

#include <glfw_cpp/glfw_cpp.hpp>
#include <glbinding/glbinding.h>

#include <print>
#include <string_view>

glfw_cpp::Instance::Unique init_glfw()
{
    auto loader = [](glfw_cpp::Api::GlContext handle, glfw_cpp::Api::GlGetProc proc) {
        glbinding::initialize(glbinding::ContextHandle(handle), proc);
    };

    auto api = glfw_cpp::Api::OpenGL{
        .m_major   = 3,
        .m_minor   = 3,
        .m_profile = glfw_cpp::Api::OpenGL::Profile::Core,
        .m_loader  = loader,
    };

    auto logger = [](glfw_cpp::Instance::LogLevel level, std::string msg) {
        using Level    = glfw_cpp::Instance::LogLevel;
        auto min_level = Level::Warning;

        if (level > min_level) {
            std::string_view name = "???";

            switch (level) {
            case Level::Warning: name = "warning"; break;
            case Level::Error: name = "error"; break;
            case Level::Critical: name = "critical"; break;
            default: [[unlikely]];
            }

            std::println(stderr, "glfw-cpp [{:>7}]: {}", name, msg);
        }
    };

    return glfw_cpp::init(api, logger);
}

int main()
{
    auto glfw   = init_glfw();
    auto wm     = glfw->createWindowManager();
    auto window = wm->createWindow({}, "Learn simple-ecs (nexus)", 1280, 720);

    window.setVsync(false);

    auto coordinator = nexus::ecs_config::Coordinator{};

    // coordinator.create_system<nexus::PhysicsSystem>();
    coordinator.create_system<nexus::RenderSystem>(
        coordinator,
        window,
        nexus::Shader{
            "./asset/shader/shader.vert",
            "./asset/shader/shader.frag",
        }
    );

    for (auto i = 0uz; i < 2; ++i) {
        auto entity = coordinator.create_entity();

        using Components = std::tuple<nexus::Gravity, nexus::RigidBody, nexus::Transform, nexus::Renderable>;
        static_assert(ecs::util::SubsetOfTuple<nexus::PhysicsSystem::Components, Components>);
        static_assert(ecs::util::SubsetOfTuple<nexus::RenderSystem::Components, Components>);

        coordinator.add_component_tuple<Components>(
            entity,
            {
                nexus::Gravity{
                    .m_force = glm::vec3{ 0.0f, -9.8f, 0.0f },
                },
                nexus::RigidBody{
                    .m_velocity     = glm::vec3{ 0.0f, static_cast<float>(i), 0.0f },
                    .m_acceleration = glm::vec3{ 0.0f, 0.0f, 0.0f },
                },
                nexus::Transform{
                    .m_position = glm::vec3{ 2.0f * i - 1.0f, 0.0f, 0.0f },
                    .m_rotation = glm::vec3{ 17.0f, -79.0f, 42.0f },
                    .m_scale    = glm::vec3{ 1.0f },
                },
                nexus::Renderable{
                    .m_color = glm::vec3{ 0.3f, 0.4f, 0.3f },
                },
            }
        );
    }

    auto timer = ecs::Timer{};
    while (wm->hasWindowOpened()) {
        wm->pollEvents();
        coordinator.update(timer.elapsed());
    }
}
