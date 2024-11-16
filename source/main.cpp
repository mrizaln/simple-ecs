#include "util/fixed_array.hpp"

#include <print>
#include <ranges>

int main()
{
    const auto fixed_array = util::FixedArray<std::string, 10>{ "simple-ecs" };
    for (const auto& [i, string] : fixed_array | std::views::reverse | std::views::enumerate) {
        std::println("{:>2} | hello: {}", i, string);
    }
}
