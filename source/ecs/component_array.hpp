#pragma once

#include "ecs/common.hpp"
#include "ecs/concepts.hpp"
#include "ecs/config.hpp"
#include "util/fixed_array.hpp"

#include <cassert>
#include <unordered_map>

namespace ecs
{
    template <concepts::Component Comp>
    class ComponentArray
    {
    public:
        using Component = Comp;

        void insert_data(Entity entity, Component component)
        {
            assert(
                not m_entity_to_index.contains(entity) and "Component added to same entity more than once"
            );

            auto index = m_size;

            // update maps
            m_entity_to_index.emplace(entity, index);
            m_index_to_entity.emplace(index, entity);

            // put new entry at end
            m_components[index] = std::move(component);

            ++m_size;
        }

        void remove_data(Entity entity)
        {
            assert(m_entity_to_index.contains(entity) and "Removing non-existent component");

            // copy element at end into deleted element's place to maintain density
            auto index_of_removed_entity = m_entity_to_index[entity];
            auto index_of_last_element   = m_size - 1;

            m_components[index_of_removed_entity] = std::move(m_components[index_of_last_element]);

            // update map to point to moved spot
            auto last_entity = m_index_to_entity[index_of_last_element];

            m_entity_to_index[last_entity]             = index_of_removed_entity;
            m_index_to_entity[index_of_removed_entity] = last_entity;

            m_entity_to_index.erase(entity);
            m_index_to_entity.erase(index_of_last_element);

            --m_size;
        }

        template <typename Self>
        auto&& get_data(this Self&& self, Entity entity)
        {
            assert(self.m_entity_to_index.contains(entity) and "Retrieving non-existent component");

            // return a reference to the entity's component
            auto index = self.m_entity_to_index.at(entity);
            return std::forward<decltype(self)>(self).m_components[index];
        }

        void entity_destroyed(Entity entity)
        {
            if (m_entity_to_index.contains(entity)) {
                remove_data(entity);
            }
        }

    private:
        // entities array
        util::FixedArray<Comp, config::max_entities> m_components = {};

        // bidirectional map of Entity and index to array
        std::unordered_map<Entity, std::size_t> m_entity_to_index = {};
        std::unordered_map<std::size_t, Entity> m_index_to_entity = {};

        // total size of valid entries in the array
        std::size_t m_size = 0;
    };
}
