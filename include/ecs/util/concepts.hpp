#pragma once

#include "ecs/util/traits.hpp"

#include <concepts>

namespace ecs::util
{
    /**
     * @brief Check whether a type can be dereferenced.
     *
     * The check only considers non-const `From`.
     * Useful to pass the const from a pointer to the object it points to.
     */
    template <typename From, typename To>
    concept Dereferencable = requires (From t) {
        { *t } -> std::same_as<To&>;
    };

    /**
     * @brief Check whether a type can be indexed.
     *
     * The check only considers non-const operator[] invocation.
     * Useful to pass const from an array-like (includeing pointer) to the object it points to.
     */
    template <typename From, typename To>
    concept Indexable = requires (From t, std::size_t i) {
        { t[i] } -> std::same_as<To&>;
    };

    template <typename... Pack>
    concept Unique = IsUnique<Pack...>::value;

    template <typename T, typename... Pack>
    concept OneOf = (std::same_as<T, Pack> or ...);

    template <typename... Pack>
    concept NonEmpty = sizeof...(Pack) > 0;

    template <std::size_t N, typename... Pack>
    concept SizeLessThan = sizeof...(Pack) < N;

    template <typename Tuple, typename... Pack>
    concept SubsetOf = TupleTraits<Tuple>::template subset_of<Pack...>;

    template <typename Tuple1, typename Tuple2>
    concept SubsetOfTuple = subset_of_tuple<Tuple1, Tuple2>();
}
