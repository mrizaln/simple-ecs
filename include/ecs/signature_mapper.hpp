#pragma once

#include "ecs/concepts.hpp"
#include "ecs/util/concepts.hpp"

namespace ecs
{
    template <concepts::Component... Comps>
        requires util::Unique<Comps...>      //
             and util::NonEmpty<Comps...>    //
             and util::SizeLessThan<config::max_components, Comps...>
    struct SignatureMapper
    {
        using Inner      = config::SignatureInner;
        using Components = std::tuple<Comps...>;

        template <concepts::Component Comp>
            requires util::OneOf<Comp, Comps...>
        static constexpr Signature map()
        {
            using Traits = util::PackTraits<Comps...>;
            auto index   = Traits::template index<Comp>();
            auto bit     = Inner{ 1 } << index;
            return Signature{ bit };
        }

        template <concepts::Component... OtherComps>
            requires (util::OneOf<OtherComps, Comps...> or ...)
        static constexpr Signature map_multiple()
        {
            return (map<OtherComps>() | ...);
        }

        template <concepts::ComponentsTuple Tuple>
            requires util::SubsetOf<Tuple, Comps...>
        static constexpr Signature map_tuple()
        {
            auto handler = []<std::size_t... Is>(std::index_sequence<Is...>) {
                return map_multiple<std::tuple_element_t<Is, Tuple>...>();
            };
            return handler(std::make_index_sequence<std::tuple_size_v<Tuple>>{});
        }
    };
}
