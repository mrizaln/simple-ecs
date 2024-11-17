#pragma once

#include "ecs/common.hpp"
#include "ecs/component_manager.hpp"
#include "ecs/concepts.hpp"
#include "ecs/entity_manager.hpp"
#include "ecs/signature_mapper.hpp"
#include "ecs/system_manager.hpp"
#include <concepts>

namespace ecs
{
    template <concepts::Component... Comps>
    class Coordinator
    {
    public:
        using EntityManager    = ecs::EntityManager;
        using ComponentManager = ComponentManager<Comps...>;
        using SystemManager    = SystemManager<Comps...>;
        using SigMapper        = SignatureMapper<Comps...>;

        Coordinator() = default;

        Coordinator(
            EntityManager&&    entity_manager,
            ComponentManager&& comp_manager,
            SystemManager&&    sys_manager
        )
            : m_entity_manager{ std::move(entity_manager) }
            , m_component_manager{ std::move(comp_manager) }
            , m_system_manager{ std::move(sys_manager) }
        {
        }

        // entity methods
        // --------------

        Entity create_entity() { return m_entity_manager.create_entity(); }

        void destroy_entity(Entity entity)
        {
            m_entity_manager.destroy_entity(entity);
            m_component_manager.entity_destroyed(entity);
            m_system_manager.entity_destroyed(entity);
        }

        // --------------

        // component methods
        // -----------------

        template <concepts::Component Comp>
        void add_component(Entity entity, Comp component)
        {
            m_component_manager.add_component(entity, component);

            auto signature = m_entity_manager.get_signature(entity);
            signature.set(SigMapper::template map<Comp>());
            m_entity_manager.set_signature(entity, signature);

            m_system_manager.entity_signature_changed(entity, signature);
        }

        template <concepts::ComponentsTuple CompsTuple>
        void add_component_tuple(Entity entity, CompsTuple&& components)
        {
            auto handler = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                (add_component(entity, std::get<Is>(components)), ...);
            };
            handler(std::make_index_sequence<std::tuple_size_v<CompsTuple>>{});
        }

        template <concepts::Component Comp>
        void remove_component(Entity entity)
        {
            m_component_manager.template remove_component<Comp>(entity);

            auto signature = m_entity_manager.get_signature(entity);
            signature.reset(SigMapper::template map<Comp>());
            m_entity_manager.set_signature(entity, signature);

            m_system_manager.entity_signature_changed(entity, signature);
        }

        template <concepts::ComponentsTuple CompsTuple>
        void remove_component_tuple(Entity entity)
        {
            auto handler = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                (remove_component<std::tuple_element_t<Is, CompsTuple>>(entity), ...);
            };
            handler(std::make_index_sequence<std::tuple_size_v<CompsTuple>>{});
        }

        template <concepts::Component Comp, typename Self>
        auto&& get_component(this Self&& self, Entity entity)
        {
            auto&& comp_manager = std::forward<Self>(self).m_component_manager;
            return comp_manager.template get_component<Comp>(entity);
        }

        template <concepts::ComponentsTuple CompsTuple, typename Self>
        auto get_component_tuple(this Self&& self, Entity entity)
        {
            auto handler = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return std::forward_as_tuple(
                    std::forward<Self>(self).template get_component<std::tuple_element_t<Is, CompsTuple>>(
                        entity
                    )...
                );
            };
            return handler(std::make_index_sequence<std::tuple_size_v<CompsTuple>>{});
        };

        template <concepts::Component Comp>
        bool has_component(Entity entity) const
        {
            auto comp_signature   = SigMapper::template map<Comp>;
            auto entity_signature = m_entity_manager.get_signature(entity);
            return (comp_signature & entity_signature) == comp_signature;
        }

        template <concepts::ComponentsTuple CompsTuple>
        bool has_component_tuple(Entity entity) const
        {
            auto comps_signature  = SigMapper::template map_tuple<CompsTuple>;
            auto entity_signature = m_entity_manager.get_signature(entity);
            return (comps_signature & entity_signature) == comps_signature;
        }

        // -----------------

        // system methods
        // --------------

        template <typename System, typename... Args>
            requires concepts::HasComponents<System>                 //
                 and std::derived_from<System, ISystem<Comps...>>    //
                 and std::constructible_from<System, Args...>
        System& create_system(Args&&... args)
        {
            return m_system_manager.template create_system<System>(std::forward<Args>(args)...);
        }

        void update(Duration frame_time) { m_system_manager.update(*this, frame_time); }

        // --------------

        // private:
        EntityManager    m_entity_manager;
        ComponentManager m_component_manager;
        SystemManager    m_system_manager;
    };
}
