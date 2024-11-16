#pragma once

#include "ecs/common.hpp"
#include "ecs/concepts.hpp"
#include "ecs/entity_manager.hpp"

namespace ecs
{
    template <typename CompManager, typename SysManager>
    class Coordinator
    {
    public:
        using EntityManager    = ecs::EntityManager;
        using ComponentManager = CompManager;
        using SystemManager    = SysManager;

        Coordinator(EntityManager&& entity_manager, CompManager&& comp_manager, SysManager&& sys_manager)
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
            signature.set(CompManager::SigMapper::template map<Comp>());
            m_entity_manager.set_signature(entity, signature);

            m_system_manager.entity_signature_changed(entity, signature);
        }

        template <concepts::Component Comp>
        void remove_component(Entity entity)
        {
            m_component_manager.template remove_component<Comp>(entity);

            auto signature = m_entity_manager.get_signature(entity);
            signature.reset(CompManager::SigMapper::template map<Comp>());
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

        template <typename Self>
        auto&& systems(this Self&& self)
        {
            return std::forward<Self>(self).m_system_manager.systems();
        }

        // --------------

    private:
        EntityManager m_entity_manager;
        CompManager   m_component_manager;
        SysManager    m_system_manager;
    };
}
