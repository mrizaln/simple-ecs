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

        /**
         * @class SignatureMapperIdent
         * @brief An empty struct to identify a signature mapper.
         *
         * Only used for concept checking. Other type should not construct this type.
         */
        struct SignatureMapperIdent
        {
        };
    }

    template <typename T>
    concept ComponentsTuple = detail::TupleOfComponents<T>::value;

    template <typename T>
    concept System = requires (T sys, Entity entity) {
        typename T::Components;
        requires ComponentsTuple<typename T::Components>;

        { sys.add_entity(entity) } -> std::same_as<void>;
        { sys.remove_entity(entity) } -> std::same_as<void>;
    };

    template <typename T>
    concept SignatureMapper = requires {
        typename T::Components;
        requires ComponentsTuple<typename T::Components>;

        { auto{ T::ident } } -> std::same_as<detail::SignatureMapperIdent>;
    };
}
