#pragma once

#include <ecs/concepts.hpp>

namespace nexus
{
    struct Player
    {
    };

    static_assert(ecs::concepts::Component<Player>);
}
