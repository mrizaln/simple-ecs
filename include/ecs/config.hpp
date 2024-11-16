#pragma once

#include <cstdint>

namespace ecs::config
{
    constexpr std::size_t max_entities   = 5000;
    constexpr std::size_t max_components = 32;

    using EntityInner    = std::uint32_t;
    using SignatureInner = std::uint32_t;
}
