#pragma once

#include "ecs/common.hpp"
#include "ecs/concepts.hpp"
#include "util/common.hpp"
#include "util/concepts.hpp"
#include "util/traits.hpp"

#include <array>

namespace ecs
{
    // non-virtual base class for all systems
    template <concepts::System... Sys>
        requires util::Unique<Sys...>
    class SystemManager
    {
    public:
        using SystemsRef  = std::tuple<Sys&...>;
        using SystemsCRef = std::tuple<Sys const&...>;

        template <concepts::SignatureMapper SigMapper>
            requires (util::SubsetOfTuple<typename Sys::Components, typename SigMapper::Components> and ...)
        static SystemManager create(Sys... sys)
        {
            auto systems    = Systems{ std::move(sys)... };
            auto signatures = Signatures{ SigMapper::template map_tuple<typename Sys::Components>()... };

            return SystemManager{ std::move(systems), std::move(signatures) };
        }

        // for some reason, using deducing this does not work here (it returns a reference to a temporary) on
        // const instance
        SystemsRef  systems() { return std::tie(get_system<Sys>()...); }
        SystemsCRef systems() const { return std::tie(get_system<Sys>()...); }

        template <util::OneOf<Sys...> S, typename Self>
        auto&& get_system(this Self&& self)
        {
            return std::get<S>(std::forward<Self>(self).m_systems);
        }

        template <util::OneOf<Sys...> S, typename Self>
        auto&& get_signature(this Self&& self)
        {
            using Traits = util::PackTraits<Sys...>;
            auto index   = Traits::template index<S>();
            return util::index<S>(std::forward<Self>(self).m_signatures, index);
        }

        void entity_destroyed(Entity entity)
        {
            // fold expression to the rescue :D
            (get_system<Sys>().remove_entity(entity), ...);
        }

        void entity_signature_changed(Entity entity, Signature entity_signature)
        {
            auto handler = [&](concepts::System auto& system, Signature system_signature) {
                if (entity_signature.test(system_signature)) {
                    system.add_entity(entity);
                } else {
                    system.remove_entity(entity);
                }
            };

            (handler(get_system<Sys>(), get_signature<Sys>()), ...);
        }

    private:
        using Systems    = std::tuple<Sys...>;
        using Signatures = std::array<Signature, sizeof...(Sys)>;

        SystemManager(Systems systems, Signatures signatures)
            : m_systems{ std::move(systems) }
            , m_signatures{ std::move(signatures) }
        {
        }

        Systems    m_systems    = {};
        Signatures m_signatures = {};
    };
}
