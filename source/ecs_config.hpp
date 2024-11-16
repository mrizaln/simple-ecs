#pragma once

#include "component/camera.hpp"
#include "component/gravity.hpp"
#include "component/player.hpp"
#include "component/renderable.hpp"
#include "component/rigid_body.hpp"
#include "component/thrust.hpp"
#include "component/transform.hpp"

#include <ecs/component_manager.hpp>
#include <ecs/system_manager.hpp>

namespace nexus::ecs_config
{
#define NEXUS_COMPONENTS Camera, Gravity, Player, Renderable, RigidBody, Thrust, Transform

    using ComponentManager = ecs::ComponentManager<NEXUS_COMPONENTS>;
    using SystemManager    = ecs::SystemManager<NEXUS_COMPONENTS>;
    using ISystem          = ecs::ISystem<NEXUS_COMPONENTS>;
    using Coordinator      = ecs::Coordinator<NEXUS_COMPONENTS>;

#undef NEXUS_COMPONENTS
}
