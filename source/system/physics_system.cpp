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
        auto dt = frame_time.count();

        for (const auto& entity : entities) {
            auto&& [gravity, rigidbody, tranform] = context.get_component_tuple<Components>(entity);

            tranform.m_position  += rigidbody.m_velocity * dt;
            rigidbody.m_velocity += gravity.m_force * dt;
        }
    }
}
