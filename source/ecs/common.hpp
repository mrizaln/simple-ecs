#pragma once

#include "ecs/concepts.hpp"
#include "ecs/config.hpp"
#include "util/concepts.hpp"
#include "util/traits.hpp"

#include <compare>
#include <functional>

namespace ecs
{
    struct Entity
    {
        using Inner = config::EntityInner;

        constexpr explicit Entity(Inner inner)
            : m_inner{ inner }
        {
        }

        std::strong_ordering operator<=>(this Entity const&, Entity const&) = default;

        Inner m_inner = 0;
    };

    struct Signature
    {
        using Inner = config::SignatureInner;

        // default signature means null a.k.a no components
        Signature() = default;

        constexpr explicit Signature(Inner inner)
            : m_inner{ inner }
        {
        }

        std::strong_ordering operator<=>(this Signature const&, Signature const&) = default;

        // clang-format off
        constexpr bool isComposite() const { return std::popcount(m_inner) > 1; }

        constexpr bool test(Signature const& other) const { return (m_inner & other.m_inner) == other.m_inner; }

        constexpr Signature& set  (Signature const& other) { m_inner |=  other.m_inner; return *this; }
        constexpr Signature& reset(Signature const& other) { m_inner &= ~other.m_inner; return *this; }
        constexpr Signature& flip (Signature const& other) { m_inner ^=  other.m_inner; return *this; }

        constexpr Signature operator|(Signature const& other) const { return Signature{ m_inner | other.m_inner }; }
        constexpr Signature operator&(Signature const& other) const { return Signature{ m_inner & other.m_inner }; }
        constexpr Signature operator^(Signature const& other) const { return Signature{ m_inner ^ other.m_inner }; }
        constexpr Signature operator~() const { return Signature{ ~m_inner }; }
        // clang-format on

        std::vector<Signature> decompose() const
        {
            std::vector<Signature> signatures;
            signatures.reserve(static_cast<std::size_t>(std::popcount(m_inner)));

            for (auto i = 0uz; i < config::max_components; ++i) {
                auto bit = Inner{ 1 } << i;
                if (test(Signature{ bit })) {
                    signatures.emplace_back(Signature{ bit });
                }
            }

            return signatures;
        }

        Inner m_inner = 0;
    };

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
    };
};

template <>
struct std::hash<ecs::Entity> : std::hash<ecs::Entity::Inner>
{
};

template <>
struct std::hash<ecs::Signature> : std::hash<ecs::Signature::Inner>
{
};
