#include "physics_system.hpp"

#include <ecs/coordinator.hpp>

#include <print>

namespace nexus
{
    void PhysicsSystem::update(
        ecs_config::Coordinator&     context,
        const std::set<ecs::Entity>& entities,
        ecs::Duration                frame_time
    )
    {
        std::println(">>> frame start");

        for (const auto& entity : entities) {
            auto& gravity   = context.get_component<Gravity>(entity);
            auto& rigidBody = context.get_component<RigidBody>(entity);
            auto& transform = context.get_component<Transform>(entity);

            auto& accel = rigidBody.m_acceleration;
            std::println("\t[{:>2}] accel: ({}, {}, {})", entity.m_inner, accel.x, accel.y, accel.z);
        }
    }
}
