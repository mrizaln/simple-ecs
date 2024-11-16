#include "component/gravity.hpp"
#include "component/rigid_body.hpp"
#include "component/transform.hpp"
#include "ecs_config.hpp"
#include "system/physics_system.hpp"

#include <ecs/common.hpp>
#include <ecs/entity_manager.hpp>
#include <ecs/coordinator.hpp>

#include <print>
#include <thread>

int main()
{
    auto coordinator = nexus::ecs_config::Coordinator{};
    coordinator.create_system<nexus::PhysicsSystem>();

    for (auto i = 0uz; i < 20; ++i) {
        auto entity = coordinator.create_entity();

        coordinator.add_component(
            entity,
            nexus::Gravity{
                .m_force = glm::vec3{ 0.0f, -9.8f, 0.0f },
            }
        );

        coordinator.add_component(
            entity,
            nexus::RigidBody{
                .m_velocity     = glm::vec3{ static_cast<float>(i) },
                .m_acceleration = glm::vec3{ static_cast<float>(42 - i) },
            }
        );

        coordinator.add_component(
            entity,
            nexus::Transform{
                .m_force = glm::vec3{ 1.0f, 2.0f, 3.0f },
            }
        );
    }

    auto counter = 100;
    auto timer   = ecs::Timer{};

    while (counter-- > 0) {
        auto elapsed = timer.elapsed();
        coordinator.update(elapsed);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
}
