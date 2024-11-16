#pragma once

#include <tuple>
#include <type_traits>
#include <utility>
namespace util
{
    template <typename...>
    struct IsUnique : std::true_type
    {
    };

    // inspired by: https://stackoverflow.com/a/47511516/16506263
    template <typename T, typename... Rest>
    struct IsUnique<T, Rest...>
        : std::bool_constant<(!std::is_same_v<T, Rest> && ...) && IsUnique<Rest...>::value>
    {
    };

    template <typename... Ts>
    struct PackTraits
    {
        static constexpr std::size_t size   = sizeof...(Ts);
        static constexpr bool        empty  = size == 0;
        static constexpr bool        unique = IsUnique<Ts...>::value;

        template <std::size_t I>
        using TypeAt = std::tuple_element_t<I, std::tuple<Ts...>>;

        template <typename T>
        static constexpr bool contains = (std::is_same_v<T, Ts> or ...);

        template <typename T>
            requires contains<T> and unique
        static constexpr std::size_t index()
        {
            auto handler = []<std::size_t... Is>(std::index_sequence<Is...>) {
                return ((std::is_same_v<T, TypeAt<Is>> ? Is : 0) + ...);
            };
            return handler(std::make_index_sequence<size>{});
        }
    };
}
