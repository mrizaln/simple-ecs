#pragma once

#include <glm/vec3.hpp>

#include <format>
#include <print>

template <>
struct std::formatter<glm::vec3> : std::formatter<float>
{
    template <typename FormatContext>
    auto format(const glm::vec3& vec, FormatContext& ctx) const
    {
        auto out = ctx.out();
        out      = std::format_to(out, "(");
        out      = std::formatter<float>::format(vec.x, ctx);
        out      = std::format_to(out, ", ");
        out      = std::formatter<float>::format(vec.y, ctx);
        out      = std::format_to(out, ", ");
        out      = std::formatter<float>::format(vec.z, ctx);
        return std::format_to(out, ")");
    }
};

namespace nexus
{
    template <typename... Args>
    void debug(std::format_string<Args...> fmt, Args&&... args)
    {
        std::println(stderr, fmt, std::forward<Args>(args)...);
    }
}
