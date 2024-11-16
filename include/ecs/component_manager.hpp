#pragma once

#include "ecs/common.hpp"
#include "ecs/component_array.hpp"
#include "ecs/concepts.hpp"
#include "ecs/signature_mapper.hpp"
#include "ecs/util/concepts.hpp"

namespace ecs
{
    template <concepts::Component... Comps>
        requires util::Unique<Comps...>
    class ComponentManager
    {
    public:
        using SigMapper  = SignatureMapper<Comps...>;
        using Components = std::tuple<Comps...>;

        ComponentManager() = default;

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
