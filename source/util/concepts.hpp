#pragma once

#include <concepts>

namespace util
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
     * Useful to psas const from an array-like (includeing pointer) to the object it points to.
     */
    template <typename From, typename To>
    concept Indexable = requires (From t, std::size_t i) {
        { t[i] } -> std::same_as<To&>;
    };
}
