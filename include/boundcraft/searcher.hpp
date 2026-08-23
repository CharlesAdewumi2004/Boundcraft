#pragma once

#include <cstddef>
#include <span>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

#include <boundcraft/details/lower-bound/lower-bound.hpp>
#include <boundcraft/details/upper-bound/upper-bound.hpp>

#include <boundcraft/policy.hpp>
#include <boundcraft/traits.hpp>

namespace boundcraft
{
    template <class Comp, class It, class V>
    concept one_way_lower =
        std::input_iterator<It> &&
        requires(Comp comp, std::iter_reference_t<It> elem, const V &v) {
            { std::invoke(comp, elem, v) } -> std::convertible_to<bool>;
        };

    template <class Comp, class It, class V>
    concept one_way_upper =
        std::input_iterator<It> &&
        requires(Comp comp, const V &v, std::iter_reference_t<It> elem) {
            { std::invoke(comp, v, elem) } -> std::convertible_to<bool>;
        };

    template <class Comp, class It, class V>
    concept strict_compare =
        std::input_iterator<It> &&
        requires(Comp comp,
                 std::iter_reference_t<It> a,
                 std::iter_reference_t<It> b,
                 const V &v) {
            { std::invoke(comp, a, b) } -> std::convertible_to<bool>; // Elem vs Elem
            { std::invoke(comp, a, v) } -> std::convertible_to<bool>; // Elem vs Key
            { std::invoke(comp, v, a) } -> std::convertible_to<bool>; // Key vs Elem
        };

    template <class Policy>
    class searcher final
    {
    public:
        searcher() = default;

        template <class It, class V>
        [[nodiscard]] constexpr It lower_bound(It first, It last, const V &value) const
        {
            return lower_bound(first, last, value, std::less<>{});
        }

        template <class It, class V, class Comp>
            requires one_way_lower<Comp, It, V>
        [[nodiscard]] constexpr It lower_bound(It first, It last, const V &value, Comp comp) const
        {
            return dispatch_lower(first, last, value, comp);
        }

        template <class It, class V, class Comp>
            requires strict_compare<Comp, It, V>
        [[nodiscard]] constexpr It lower_bound_strict(It first, It last, const V &value, Comp comp) const
        {
            return dispatch_lower(first, last, value, comp);
        }

        template <class It, class V>
        [[nodiscard]] constexpr It upper_bound(It first, It last, const V &value) const
        {
            return upper_bound(first, last, value, std::less<>{});
        }

        template <class It, class V, class Comp>
            requires one_way_upper<Comp, It, V>
        [[nodiscard]] constexpr It upper_bound(It first, It last, const V &value, Comp comp) const
        {
            return dispatch_upper(first, last, value, comp);
        }

        template <class It, class V, class Comp>
            requires strict_compare<Comp, It, V>
        [[nodiscard]] constexpr It upper_bound_strict(It first, It last, const V &value, Comp comp) const
        {
            return dispatch_upper(first, last, value, comp);
        }

        template <class T, class V>
        [[nodiscard]] constexpr T *lower_bound(std::span<T> s, const V &value) const
        {
            return lower_bound(s.data(), s.data() + s.size(), value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires one_way_lower<Comp, T *, V>
        [[nodiscard]] constexpr T *lower_bound(std::span<T> s, const V &value, Comp comp) const
        {
            return dispatch_lower(s.data(), s.data() + s.size(), value, comp);
        }

        template <class T, class V>
        [[nodiscard]] constexpr const T *lower_bound(std::span<const T> s, const V &value) const
        {
            return lower_bound(s.data(), s.data() + s.size(), value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires one_way_lower<Comp, const T *, V>
        [[nodiscard]] constexpr const T *lower_bound(std::span<const T> s, const V &value, Comp comp) const
        {
            return dispatch_lower(s.data(), s.data() + s.size(), value, comp);
        }

        template <class T, class V>
        [[nodiscard]] constexpr T *upper_bound(std::span<T> s, const V &value) const
        {
            return upper_bound(s.data(), s.data() + s.size(), value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires one_way_upper<Comp, T *, V>
        [[nodiscard]] constexpr T *upper_bound(std::span<T> s, const V &value, Comp comp) const
        {
            return dispatch_upper(s.data(), s.data() + s.size(), value, comp);
        }

        template <class T, class V>
        [[nodiscard]] constexpr const T *upper_bound(std::span<const T> s, const V &value) const
        {
            return upper_bound(s.data(), s.data() + s.size(), value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires one_way_upper<Comp, const T *, V>
        [[nodiscard]] constexpr const T *upper_bound(std::span<const T> s, const V &value, Comp comp) const
        {
            return dispatch_upper(s.data(), s.data() + s.size(), value, comp);
        }

        template <class T, class V>
        [[nodiscard]] constexpr T *lower_bound(T *first, T *last, const V &value) const
        {
            return lower_bound(first, last, value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires one_way_lower<Comp, T *, V>
        [[nodiscard]] constexpr T *lower_bound(T *first, T *last, const V &value, Comp comp) const
        {
            return dispatch_lower(first, last, value, comp);
        }

        template <class T, class V>
        [[nodiscard]] constexpr const T *lower_bound(const T *first, const T *last, const V &value) const
        {
            return lower_bound(first, last, value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires one_way_lower<Comp, const T *, V>
        [[nodiscard]] constexpr const T *lower_bound(const T *first, const T *last, const V &value, Comp comp) const
        {
            return dispatch_lower(first, last, value, comp);
        }

        template <class T, class V>
        [[nodiscard]] constexpr T *upper_bound(T *first, T *last, const V &value) const
        {
            return upper_bound(first, last, value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires one_way_upper<Comp, T *, V>
        [[nodiscard]] constexpr T *upper_bound(T *first, T *last, const V &value, Comp comp) const
        {
            return dispatch_upper(first, last, value, comp);
        }

        template <class T, class V>
        [[nodiscard]] constexpr const T *upper_bound(const T *first, const T *last, const V &value) const
        {
            return upper_bound(first, last, value, std::less<>{});
        }

        template <class T, class V, class Comp>
            requires one_way_upper<Comp, const T *, V>
        [[nodiscard]] constexpr const T *upper_bound(const T *first, const T *last, const V &value, Comp comp) const
        {
            return dispatch_upper(first, last, value, comp);
        }

    private:
        template <class It, class V, class Comp>
            requires one_way_lower<Comp, It, V>
        static constexpr It dispatch_lower(It first, It last, const V &value, Comp comp);

        template <class It, class V, class Comp>
            requires one_way_upper<Comp, It, V>
        static constexpr It dispatch_upper(It first, It last, const V &value, Comp comp);
    };

    template <class Policy>
    template <class It, class V, class Comp>
        requires one_way_lower<Comp, It, V>
    constexpr It searcher<Policy>::dispatch_lower(It first, It last, const V &value, Comp comp)
    {
        using traits = boundcraft::policy::traits::policy_traits<Policy>;
        constexpr auto k = traits::kind;

        if constexpr (k == policy_kind::standard_binary)
        {
            return boundcraft::detail::lower_bound_standard_binary_impl(first, last, value, comp);
        }
        else if constexpr (k == policy_kind::hybrid)
        {
            constexpr std::size_t threshold = traits::threshold;
            return boundcraft::detail::lower_bound_hybrid_impl(threshold, first, last, value, comp);
        }
        else if constexpr (k == policy_kind::galloping)
        {
            using search_policy_t = typename traits::search_policy;
            using gallop_start_t = typename traits::gallop_start;

            return boundcraft::detail::lower_bound_gallop_impl<search_policy_t, gallop_start_t>(
                first, last, value, comp);
        }
        else
        {
            static_assert([]{ return false; }(), "Unknown policy");
        }
    }

    template <class Policy>
    template <class It, class V, class Comp>
        requires one_way_upper<Comp, It, V>
    constexpr It searcher<Policy>::dispatch_upper(It first, It last, const V &value, Comp comp)
    {
        using traits = boundcraft::policy::traits::policy_traits<Policy>;
        constexpr auto k = traits::kind;

        if constexpr (k == policy_kind::standard_binary)
        {
            return boundcraft::detail::upper_bound_standard_binary_impl(first, last, value, comp);
        }
        else if constexpr (k == policy_kind::hybrid)
        {
            constexpr std::size_t threshold = traits::threshold;
            return boundcraft::detail::upper_bound_hybrid_impl(threshold, first, last, value, comp);
        }
        else if constexpr (k == policy_kind::galloping)
        {
            using search_policy_t = typename traits::search_policy;
            using gallop_start_t = typename traits::gallop_start;

            return boundcraft::detail::upper_bound_gallop_impl<search_policy_t, gallop_start_t>(
                first, last, value, comp);
        }
        else
        {
            static_assert([]
                          { return false; }(), "Unknown policy");
        }
    }

}
