#pragma once

#include "ecs/util/common.hpp"

#include <memory>
#include <utility>

namespace ecs::util
{
    /**
     * @brief A simple wrapper for `std::unique_ptr<T[]>` that remembers its size (constexpr).
     *
     * @tparam T Types to be stored stored inside the array.
     */
    template <typename T, std::size_t N>
        requires std::default_initializable<T> and std::copyable<T>
    struct FixedArray
    {
    public:
        FixedArray()
            : FixedArray(T{})
        {
        }

        FixedArray(T default_value)
            : m_data{ std::make_unique<T[]>(N) }
        {
            for (auto i = 0uz; i < N; ++i) {
                m_data[i] = default_value;
            }
        }

        static constexpr std::size_t size() noexcept { return N; }

        template <typename Self>
        auto&& operator[](this Self&& self, std::size_t index) noexcept
        {
            return util::index<T>(std::forward<Self>(self).m_data, index);
        }

        template <typename Self>
        auto* begin(this Self&& self) noexcept
        {
            return &std::forward<Self>(self)[0];
        }

        template <typename Self>
        auto* end(this Self&& self) noexcept
        {
            return &std::forward<Self>(self)[N];
        }

        const T* cbegin(this auto const& self) { return self.begin(); }
        const T* cend(this auto const& self) { return self.end(); }

    private:
        std::unique_ptr<T[]> m_data;
    };
}
