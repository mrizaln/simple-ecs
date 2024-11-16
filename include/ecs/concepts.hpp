#pragma once

#include "ecs/common.hpp"

#include <concepts>
#include <type_traits>

namespace ecs::concepts
{
    // A component is a regular type that can be trivially moved, trivially copied, and trivially assigned.
    // It can be not trivially default constructible, because user most of the time want a struct to have a
    // reasonable default value instead of a garbage value.
    template <typename T>
    concept Component = std::semiregular<T>                          //
                    and std::is_trivially_move_constructible_v<T>    //
                    and std::is_trivially_move_assignable_v<T>       //
                    and std::is_trivially_copy_constructible_v<T>    //
                    and std::is_trivially_copy_assignable_v<T>       //
                    and std::is_trivially_destructible_v<T>;

    namespace detail
    {
        template <typename>
        struct TupleOfComponents : std::false_type
        {
        };

        template <Component... Comps>
        struct TupleOfComponents<std::tuple<Comps...>> : std::true_type
        {
        };
    }

    template <typename T>
    concept ComponentsTuple = detail::TupleOfComponents<T>::value;

    template <typename T>
    concept HasComponents = requires (T sys, Entity entity) {
        typename T::Components;
        requires ComponentsTuple<typename T::Components>;
    };
}
