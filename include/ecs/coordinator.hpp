#pragma once

#include "ecs/common.hpp"
#include "ecs/component_manager.hpp"
#include "ecs/concepts.hpp"
#include "ecs/entity_manager.hpp"
#include "ecs/signature_mapper.hpp"
#include "ecs/system_manager.hpp"

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

        template <concepts::Component Comp>
        void remove_component(Entity entity)
        {
            m_component_manager.template remove_component<Comp>(entity);

            auto signature = m_entity_manager.get_signature(entity);
            signature.reset(SigMapper::template map<Comp>());
            m_entity_manager.set_signature(entity, signature);

            m_system_manager.entity_signature_changed(entity, signature);
        }

        template <concepts::Component Comp>
        Comp& get_component(Entity entity)
        {
            return m_component_manager.template get_component<Comp>(entity);
        }

        // -----------------

        // system methods
        // --------------

        template <typename System, typename... Args>
            requires concepts::HasComponents<System> and std::derived_from<System, ISystem<Comps...>>
        void create_system(Args&&... args)
        {
            m_system_manager.template create_system<System>(std::forward<Args>(args)...);
        }

        void update(Duration frame_time) { m_system_manager.update(*this, frame_time); }

        // --------------

    private:
        EntityManager    m_entity_manager;
        ComponentManager m_component_manager;
        SystemManager    m_system_manager;
    };
}
