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
            auto&& [gravity, rigidbody, transform] = context.get_component_tuple<Components>(entity);

            // translation
            transform.m_position += rigidbody.m_velocity * dt;
            rigidbody.m_velocity += rigidbody.m_acceleration * dt;
            rigidbody.m_velocity += gravity.m_force * dt;

            // rotation
            auto omega           = glm::quat{ 0.0f, rigidbody.m_angular_velocity };
            auto quat_delta      = transform.m_rotation * omega * 0.5f * dt;
            transform.m_rotation = glm::normalize(transform.m_rotation + quat_delta);
        }
    }
}
