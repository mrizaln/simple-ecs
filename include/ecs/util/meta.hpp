#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace ecs::util::detail
{
    // https://stackoverflow.com/a/57528226/16506263
    template <typename T, typename... Ts>
    struct TupleUniqPackImpl
    {
        using Type = T;
    };

    // clang-format off
    template <typename... Ts, typename U, typename... Us>
    struct TupleUniqPackImpl<std::tuple<Ts...>, U, Us...> :
        std::conditional_t<
            (std::is_same_v<U, Ts> || ...),
            TupleUniqPackImpl<std::tuple<Ts...>, Us...>,
            TupleUniqPackImpl<std::tuple<Ts..., U>, Us...>>
    {
    };
    // clang-format on

    template <typename, typename>
    struct TupleCatImpl
    {
        static_assert(false, "either (or both) types are not tuple");
    };

    template <typename... Ts, typename... Us>
    struct TupleCatImpl<std::tuple<Ts...>, std::tuple<Us...>>
    {
        using Type = std::tuple<Ts..., Us...>;
    };

    template <typename>
    struct TupleUniqImpl
    {
        static_assert(false, "not a tuple");
    };

    template <typename... Ts>
    struct TupleUniqImpl<std::tuple<Ts...>> : detail::TupleUniqPackImpl<std::tuple<>, Ts...>
    {
    };

    template <typename, typename>
    struct TupleCatUniqImpl
    {
        static_assert(false, "either (or both) types are not tuple");
    };

    template <typename... Ts, typename... Us>
    struct TupleCatUniqImpl<std::tuple<Ts...>, std::tuple<Us...>>
    {
        using Type = TupleUniqImpl<typename TupleCatImpl<std::tuple<Ts...>, std::tuple<Us...>>::Type>::Type;
    };
}

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
        static_assert(false, "not a tuple");
    };

    template <typename... Ts>
    struct TupleTraits<std::tuple<Ts...>> : PackTraits<Ts...>
    {
    };

    template <typename Tuple1>
    using TupleUniq = typename detail::TupleUniqImpl<Tuple1>::Type;

    template <typename Tuple1, typename Tuple2>
    using TupleCat = typename detail::TupleCatImpl<Tuple1, Tuple2>::Type;

    template <typename Tuple1, typename Tuple2>
    using TupleCatUniq = typename detail::TupleCatUniqImpl<Tuple1, Tuple2>::Type;

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
}
