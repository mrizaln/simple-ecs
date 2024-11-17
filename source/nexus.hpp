#pragma once

#include "component/renderable.hpp"
#include "ecs/util/concepts.hpp"
#include "component/gravity.hpp"
#include "component/rigid_body.hpp"
#include "component/transform.hpp"
#include "graphics/shader.hpp"
#include "system/physics_system.hpp"
#include "system/render_system.hpp"
#include "ecs_config.hpp"

#include <ecs/common.hpp>
#include <ecs/entity_manager.hpp>
#include <ecs/coordinator.hpp>

#include <glfw_cpp/glfw_cpp.hpp>
#include <glbinding/glbinding.h>

#include <concepts>
#include <print>
#include <random>
#include <string_view>

namespace nexus
{
    inline glfw_cpp::Instance::Unique init_glfw()
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

    class Nexus
    {
    public:
        Nexus(std::string_view title, int width, int height)
            : m_glfw{ init_glfw() }
            , m_wm{ m_glfw->createWindowManager() }
            , m_window{ m_wm->createWindow({}, title, width, height) }
            , m_coordinator{}
        {
            m_window.setVsync(true);

            m_coordinator.create_system<nexus::PhysicsSystem>();
            m_coordinator.create_system<nexus::RenderSystem>(
                m_coordinator,
                m_window,
                nexus::Shader{
                    "./asset/shader/shader.vert",
                    "./asset/shader/shader.frag",
                }
            );
        }

        void run()
        {
            using AllComponents = std::tuple<
                nexus::Gravity,    //
                nexus::RigidBody,
                nexus::Transform,
                nexus::Renderable>;

            static_assert(ecs::util::SubsetOfTuple<nexus::PhysicsSystem::Components, AllComponents>);
            static_assert(ecs::util::SubsetOfTuple<nexus::RenderSystem::Components, AllComponents>);

            auto rand_pos   = RandomGenerator{ -125.0f, 125.0f };
            auto rand_rot   = RandomGenerator{ 0.0f, 3.14f };
            auto rand_vel   = RandomGenerator{ -100.0f, 100.0f };
            auto rand_scale = RandomGenerator{ 1.0f, 4.0f };
            auto rand_color = RandomGenerator{ 0.0f, 1.0f };

            auto grav = [&rand_scale](float scale) {
                auto range = rand_scale.range();
                return glm::vec3{ 0.0f, -9.8f * scale / range, 0.0f };
            };

            for (auto i = 0uz; i < ecs::config::max_entities - 1; ++i) {
                auto entity = m_coordinator.create_entity();

                auto scale    = rand_scale();
                auto vel      = [&]() { return rand_vel() / scale; };
                auto avel     = [&]() { return rand_rot() / scale; };
                auto vel_half = [&]() {
                    auto range = rand_scale.range();
                    return (rand_vel() + range / 2.0f) / scale;
                };

                m_coordinator.add_component_tuple<AllComponents>(
                    entity,
                    {
                        nexus::Gravity{
                            .m_force = grav(scale),
                        },
                        nexus::RigidBody{
                            .m_velocity         = glm::vec3{ vel(), vel_half(), vel() },
                            .m_acceleration     = glm::vec3{ 0.0f, 0.0f, 0.0f },
                            .m_angular_velocity = glm::vec3{ avel(), avel(), avel() },
                        },
                        nexus::Transform{
                            .m_position = glm::vec3{ rand_pos(), rand_pos(), rand_pos() },
                            .m_scale    = glm::vec3{ scale },
                            .m_rotation = glm::vec3{ rand_rot(), rand_rot(), rand_rot() },
                        },
                        nexus::Renderable{
                            .m_color = glm::vec3{ rand_color(), rand_color(), rand_color() },
                        },
                    }
                );
            }

            // reset timer
            m_timer.elapsed();

            while (m_wm->hasWindowOpened()) {
                auto elapsed = m_timer.elapsed();
                m_coordinator.update(elapsed);
                m_wm->pollEvents();

                std::println("Frame time: {}", elapsed);
            }
        }

    private:
        template <std::floating_point T>
        struct RandomGenerator
        {
        public:
            RandomGenerator(T min, T max)
                : m_dist{ min, max }
            {
            }

            T operator()() { return m_dist(m_rng); }
            T min() { return m_dist.min(); }
            T max() { return m_dist.max(); }
            T range() { return max() - min(); }

        private:
            std::mt19937                      m_rng = std::mt19937{ std::random_device{}() };
            std::uniform_real_distribution<T> m_dist;
        };

        glfw_cpp::Instance::Unique      m_glfw;
        glfw_cpp::WindowManager::Shared m_wm;
        glfw_cpp::Window                m_window;

        ecs::Timer              m_timer;
        ecs_config::Coordinator m_coordinator;
    };
}
