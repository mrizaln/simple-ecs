#pragma once

#include "ecs/common.hpp"
#include "ecs/component_array.hpp"
#include "ecs/concepts.hpp"
#include "util/concepts.hpp"

namespace ecs
{
    template <concepts::Component... Comps>
        requires util::Unique<Comps...>      //
             and util::NonEmpty<Comps...>    //
             and util::SizeLessThan<config::max_components, Comps...>
    struct SignatureMapper
    {
        using Inner = config::SignatureInner;

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
    };

    template <concepts::Component... Comps>
        requires util::Unique<Comps...>
    class ComponentManager
    {
    public:
        using SigMapper  = SignatureMapper<Comps...>;
        using Components = std::tuple<Comps...>;

        ComponentManager() = default;

        template <util::OneOf<Comps...>... OtherComps>
        static constexpr Signature get_component_signature()
        {
            return SigMapper::template map_multiple<OtherComps...>();
        }

        template <concepts::ComponentsTuple Tuple>
            requires util::SubsetOf<Tuple, Comps...>
        static constexpr Signature get_component_signature_tup()
        {
            auto handler = []<std::size_t... Is>(std::index_sequence<Is...>) {
                return get_component_signature<std::tuple_element_t<Is, Tuple>...>();
            };
            return handler(std::make_index_sequence<std::tuple_size_v<Tuple>>{});
        }

        template <util::OneOf<Comps...> Comp>
        void add_component(Entity entity, Comp component)
        {
            auto& comp_array = get_component_array<Comp>();
            comp_array.insert_data(entity, component);
        }

        template <util::OneOf<Comps...> Comp>
        void remove_component(Entity entity)
        {
            auto& comp_array = get_component_array<Comp>();
            comp_array.remove_data(entity);
        }

        template <util::OneOf<Comps...> Comp, typename Self>
        Comp&& get_component(this Self&& self, Entity entity)
        {
            auto&& comp_array = std::forward<Self>(self).template get_component_array<Comp>();
            return comp_array.get_data(entity);
        }

        void entity_destroyed(Entity entity)
        {
            // fold expression to the rescue :D
            (get_component_array<Comps>().remove_data(entity), ...);
        }

    private:
        using ComponentArrays = std::tuple<ComponentArray<Comps>...>;

        template <util::OneOf<Comps...> Comp, typename Self>
        auto&& get_component_array(this Self&& self)
        {
            auto&& components = std::forward<decltype(self)>(self).m_component_arrays;
            return std::get<ComponentArray<Comp>>(components);
        }

        ComponentArrays m_component_arrays;
    };
}
