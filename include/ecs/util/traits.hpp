#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace ecs::util
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
        template <std::size_t I>
        using TypeAt = std::tuple_element_t<I, std::tuple<Ts...>>;

        static constexpr std::size_t size   = sizeof...(Ts);
        static constexpr bool        empty  = size == 0;
        static constexpr bool        unique = IsUnique<Ts...>::value;

        template <typename T>
        static constexpr bool contains = (std::is_same_v<T, Ts> or ...);

        template <typename... Us>
        static constexpr bool subset_of = (PackTraits<Us...>::template contains<Ts> and ...);

        template <typename... Us>
        static constexpr bool subset_of_tuple()
        {
            auto handler = []<std::size_t... Is>(std::index_sequence<Is...>) {
                return (PackTraits<Us...>::template contains<TypeAt<Is>> and ...);
            };
            return handler(std::make_index_sequence<size>{});
        }

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

    template <typename>
    struct TupleTraits
    {
        static_assert(false, "TupleTraits only supports std::tuple");
    };

    template <typename... Ts>
    struct TupleTraits<std::tuple<Ts...>> : PackTraits<Ts...>
    {
    };

    template <typename Tuple1, typename Tuple2>
    static constexpr bool subset_of_tuple()
    {
        using Traits1 = TupleTraits<Tuple1>;
        using Traits2 = TupleTraits<Tuple2>;

        auto handler = []<std::size_t... Is>(std::index_sequence<Is...>) {
            return (Traits2::template contains<typename Traits1::template TypeAt<Is>> and ...);
        };

        return handler(std::make_index_sequence<Traits1::size>{});
    }

    template <typename>
    struct TupleRef
    {
        static_assert(false, "Not a tuple");
    };

    template <typename... Ts>
    struct TupleRef<std::tuple<Ts...>>
    {
        using Type = std::tuple<Ts&...>;
    };
}
