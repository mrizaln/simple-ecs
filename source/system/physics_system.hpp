#pragma once

#include "component/gravity.hpp"
#include "component/rigid_body.hpp"
#include "component/transform.hpp"
#include "ecs_config.hpp"

#include <ecs/common.hpp>
#include <ecs/concepts.hpp>

#include <set>
#include <tuple>

namespace nexus
{
    class PhysicsSystem final : public ecs_config::ISystem
    {
    public:
        using Components = std::tuple<Gravity, RigidBody, Transform>;

        PhysicsSystem()           = default;
        ~PhysicsSystem() override = default;

        void update(
            ecs_config::Coordinator&     context,
            const std::set<ecs::Entity>& entities,
            ecs::Duration                frame_time
        ) override;
    };
}
