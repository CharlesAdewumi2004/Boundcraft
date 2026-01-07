#pragma once

#include <cstddef>
#include <span>
#include <functional>
#include <iterator>

#include "details/lower-bound-gallop-impl.hpp"
#include "details/lower-bound-hybrid-impl.hpp"
#include "details/lower-bound-standard-impl.hpp"
#include "policy.hpp"
#include "traits.hpp"

namespace boundcraft
{
    template <class Comp, class It, class V>
    concept lower_bound_comparator =
        std::indirect_strict_weak_order<Comp, It, const V *>;

    template <class Policy>
    class searcher final
    {
    public:
        searcher() = default;
        ~searcher() = default;

        // ------------------------------------------------------------
        // Iterators
        // ------------------------------------------------------------
        template <class It, class V>
        It lower_bound(It first, It last, const V &value);

        template <class It, class V, class Comp>
            requires lower_bound_comparator<Comp, It, V>
        It lower_bound(It first, It last, const V &value, Comp comp);

        // ------------------------------------------------------------
        // std::span
        // ------------------------------------------------------------
        template <class T, class V>
        T *lower_bound(std::span<T> s, const V &value);

        template <class T, class V, class Comp>
            requires lower_bound_comparator<Comp, T *, V>
        T *lower_bound(std::span<T> s, const V &value, Comp comp);

        template <class T, class V>
        const T *lower_bound(std::span<const T> s, const V &value);

        template <class T, class V, class Comp>
            requires lower_bound_comparator<Comp, const T *, V>
        const T *lower_bound(std::span<const T> s, const V &value, Comp comp);

        // ------------------------------------------------------------
        // Pointers
        // ------------------------------------------------------------
        template <class T, class V>
        T *lower_bound(T *first, T *last, const V &value);

        template <class T, class V, class Comp>
            requires lower_bound_comparator<Comp, T *, V>
        T *lower_bound(T *first, T *last, const V &value, Comp comp);

        template <class T, class V>
        const T *lower_bound(const T *first, const T *last, const V &value);

        template <class T, class V, class Comp>
            requires lower_bound_comparator<Comp, const T *, V>
        const T *lower_bound(const T *first, const T *last, const V &value, Comp comp);

    private:
        // single policy switch so gallop changes apply everywhere
        template <class It, class V, class Comp>
            requires lower_bound_comparator<Comp, It, V>
        static It dispatch(It first, It last, const V &value, Comp comp);
    };

    // =========================================================================
    // Iterators
    // =========================================================================

    template <class Policy>
    template <class It, class V>
    It searcher<Policy>::lower_bound(It first, It last, const V &value)
    {
        return lower_bound(first, last, value, std::less<>{});
    }

    template <class Policy>
    template <class It, class V, class Comp>
        requires lower_bound_comparator<Comp, It, V>
    It searcher<Policy>::lower_bound(It first, It last, const V &value, Comp comp)
    {
        return dispatch(first, last, value, comp);
    }

    // =========================================================================
    // std::span
    // =========================================================================

    template <class Policy>
    template <class T, class V>
    T *searcher<Policy>::lower_bound(std::span<T> s, const V &value)
    {
        return lower_bound(s, value, std::less<>{});
    }

    template <class Policy>
    template <class T, class V, class Comp>
        requires lower_bound_comparator<Comp, T *, V>
    T *searcher<Policy>::lower_bound(std::span<T> s, const V &value, Comp comp)
    {
        return dispatch(s.data(), s.data() + s.size(), value, comp);
    }

    template <class Policy>
    template <class T, class V>
    const T *searcher<Policy>::lower_bound(std::span<const T> s, const V &value)
    {
        return lower_bound(s, value, std::less<>{});
    }

    template <class Policy>
    template <class T, class V, class Comp>
        requires lower_bound_comparator<Comp, const T *, V>
    const T *searcher<Policy>::lower_bound(std::span<const T> s, const V &value, Comp comp)
    {
        return dispatch(s.data(), s.data() + s.size(), value, comp);
    }

    // =========================================================================
    // Pointers
    // =========================================================================

    template <class Policy>
    template <class T, class V>
    T *searcher<Policy>::lower_bound(T *first, T *last, const V &value)
    {
        return lower_bound(first, last, value, std::less<>{});
    }

    template <class Policy>
    template <class T, class V, class Comp>
        requires lower_bound_comparator<Comp, T *, V>
    T *searcher<Policy>::lower_bound(T *first, T *last, const V &value, Comp comp)
    {
        return dispatch(first, last, value, comp);
    }

    template <class Policy>
    template <class T, class V>
    const T *searcher<Policy>::lower_bound(const T *first, const T *last, const V &value)
    {
        return lower_bound(first, last, value, std::less<>{});
    }

    template <class Policy>
    template <class T, class V, class Comp>
        requires lower_bound_comparator<Comp, const T *, V>
    const T *searcher<Policy>::lower_bound(const T *first, const T *last, const V &value, Comp comp)
    {
        return dispatch(first, last, value, comp);
    }

    // =========================================================================
    // Dispatch (single policy switch)
    // =========================================================================

    template <class Policy>
    template <class It, class V, class Comp>
        requires lower_bound_comparator<Comp, It, V>
    It searcher<Policy>::dispatch(It first, It last, const V &value, Comp comp)
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

            return boundcraft::detail::lower_bound_gallop_impl<
                search_policy_t,
                gallop_start_t>(first, last, value, comp);
        }
        else
        {
            static_assert([]{ return false; }(), "Unknown policy");
        }
    }

}
