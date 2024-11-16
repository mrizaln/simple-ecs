#pragma once

#include "ecs/common.hpp"
#include "ecs/util/fixed_array.hpp"

#include <cassert>
#include <queue>

namespace ecs
{
    class EntityManager
    {
    public:
        EntityManager()
        {
            // initialize the queue with all possible entity IDs
            for (Entity::Inner counter = 0; counter < config::max_entities; ++counter) {
                m_available_entities.emplace(counter);
            }
        }

        Entity create_entity()
        {
            assert(m_living_entity_count < config::max_entities and "Too many entities in existence");

            // take an id from the front of the queue
            auto id = m_available_entities.front();
            m_available_entities.pop();

            ++m_living_entity_count;

            return id;
        }

        void destroy_entity(Entity entity)
        {
            assert(entity.m_inner < config::max_entities and "Entity out of range");

            // invalidate the destroyed entity's signature
            m_signatures[entity.m_inner].reset(Signature{});

            // put the destoryed id at the back of the queue
            m_available_entities.push(entity);
            --m_living_entity_count;
        }

        void set_signature(Entity entity, Signature signature)
        {
            assert(entity.m_inner < config::max_entities and "Entity out of range");

            // put this entity's signature into the array
            m_signatures[entity.m_inner] = signature;
        }

        Signature get_signature(Entity entity) const
        {
            assert(entity.m_inner < config::max_entities and "Entity out of range");

            // get the entity's signature from the array
            return m_signatures[entity.m_inner];
        }

    private:
        std::queue<Entity>                                m_available_entities = {};
        util::FixedArray<Signature, config::max_entities> m_signatures         = {};

        Entity::Inner m_living_entity_count = 0;
    };
}
