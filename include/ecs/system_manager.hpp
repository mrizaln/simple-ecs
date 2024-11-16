#pragma once

#include "ecs/common.hpp"
#include "ecs/concepts.hpp"
#include "ecs/signature_mapper.hpp"

#include <memory>
#include <ranges>
#include <set>

namespace ecs
{
    template <concepts::Component... Comps>
    class SystemManager;

    template <concepts::Component... Comps>
    class Coordinator;

    template <concepts::Component... Comps>
    class ISystem
    {
    public:
        virtual void update(
            Coordinator<Comps...>&  context,
            const std::set<Entity>& entities,
            Duration                frame_time
        ) = 0;

        virtual ~ISystem() = default;
    };

    template <concepts::Component... Comps>
    class SystemManager
    {
    public:
        SystemManager() = default;

        template <typename System, typename... Args>
            requires concepts::HasComponents<System> and std::derived_from<System, ISystem<Comps...>>
        void create_system(Args&&... args)
        {
            using SigMapper = SignatureMapper<Comps...>;

            m_systems.emplace_back(
                SigMapper::template map_tuple<typename System::Components>(),
                std::make_unique<System>(std::forward<Args>(args)...)
            );
            m_entities.emplace_back();
        }

        void entity_destroyed(Entity entity)
        {
            for (auto& entities : m_entities) {
                entities.erase(entity);
            }
        }

        void entity_signature_changed(Entity entity, Signature entity_signature)
        {
            for (auto i : std::views::iota(0uz, m_systems.size())) {
                auto& [system_signature, system] = m_systems[i];
                if (entity_signature.test(system_signature)) {
                    m_entities[i].insert(entity);
                } else {
                    m_entities[i].erase(entity);
                }
            }
        }

        void update(Coordinator<Comps...>& context, Duration frame_time)
        {
            for (auto i : std::views::iota(0uz, m_systems.size())) {
                auto& [_, system] = m_systems[i];
                auto& entities    = m_entities[i];

                system->update(context, entities, frame_time);
            }
        }

    private:
        using ISystem = ISystem<Comps...>;

        struct SystemInfo
        {
            Signature                m_signature;
            std::unique_ptr<ISystem> m_system;
        };

        std::vector<SystemInfo>       m_systems;
        std::vector<std::set<Entity>> m_entities;
    };
}
