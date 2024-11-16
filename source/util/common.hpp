#pragma once

#include "util/concepts.hpp"

#include <utility>

namespace util
{
    /**
     * @brief Derefernce a pointer while preserving constness.
     *
     * @param from The object to dereference.
     *
     * @return The dereferenced object (reference), const if `from` is const.
     */
    template <typename To, Dereferencable<To> From>
    decltype(auto) deref(From&& from)
    {
        if constexpr (std::is_const_v<std::remove_reference_t<From>>) {
            return std::as_const(*from);
        } else {
            return *from;
        }
    }

    /**
     * @brief Index into an indexable object while preserving constness.
     *
     * @param from The object to index.
     * @param i The index to use.
     *
     * @return The indexed object (reference), const if `from` is const.
     */
    template <typename To, Indexable<To> From>
    decltype(auto) index(From&& from, std::size_t i)
    {
        if constexpr (std::is_const_v<std::remove_reference_t<From>>) {
            return std::as_const(from[i]);
        } else {
            return from[i];
        }
    }
}
