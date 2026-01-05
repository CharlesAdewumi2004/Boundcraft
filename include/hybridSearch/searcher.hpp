#pragma once

#include <cstddef>
#include <span>
#include <functional>
#include <iterator>   // std::indirect_strict_weak_order, iterator concepts/traits

#include "details/lower-bound-gallop-hybrid-impl.hpp" // left included for now (you'll decide gallop wiring)
#include "details/lower-bound-gallop-impl.hpp"        // left included for now (you'll decide gallop wiring)
#include "details/lower-bound-hybrid-impl.hpp"
#include "details/lower-bound-standard-impl.hpp"
#include "policy.hpp"
#include "traits.hpp"

namespace hybrid_search
{
    template <class Comp, class It, class V>
    concept lower_bound_comparator =
        std::indirect_strict_weak_order<Comp, It, const V*>;

    template <class Policy>
    class searcher final
    {
    public:
        searcher() = default;
        ~searcher() = default;

        template <class It, class V>
        It lower_bound(It first, It last, const V& value);

        template <class It, class V, class Comp>
        requires lower_bound_comparator<Comp, It, V>
        It lower_bound(It first, It last, const V& value, Comp comp);

        template <class T, class V>
        T* lower_bound(std::span<T> s, const V& value);

        template <class T, class V, class Comp>
        requires lower_bound_comparator<Comp, T*, V>
        T* lower_bound(std::span<T> s, const V& value, Comp comp);

        template <class T, class V>
        const T* lower_bound(std::span<const T> s, const V& value);

        template <class T, class V, class Comp>
        requires lower_bound_comparator<Comp, const T*, V>
        const T* lower_bound(std::span<const T> s, const V& value, Comp comp);

        template <class T, class V>
        T* lower_bound(T* first, T* last, const V& value);

        template <class T, class V, class Comp>
        requires lower_bound_comparator<Comp, T*, V>
        T* lower_bound(T* first, T* last, const V& value, Comp comp);

        template <class T, class V>
        const T* lower_bound(const T* first, const T* last, const V& value);

        template <class T, class V, class Comp>
        requires lower_bound_comparator<Comp, const T*, V>
        const T* lower_bound(const T* first, const T* last, const V& value, Comp comp);

    private:
        using search_policy = Policy;
        std::size_t last_idx = 0;
        bool has_hint = false;
    };


    template <class Policy>
    template <class It, class V>
    It searcher<Policy>::lower_bound(It first, It last, const V& value)
    {
        return lower_bound(first, last, value, std::less<>{});
    }

    template <class Policy>
    template <class It, class V, class Comp>
    requires lower_bound_comparator<Comp, It, V>
    It searcher<Policy>::lower_bound(It first, It last, const V& value, Comp comp)
    {
        constexpr auto k = hybrid_search::policy::traits::policy_traits<Policy>::kind;

        if constexpr (k == policy_kind::standard_binary)
        {
            return hybrid_search::detail::lower_bound_standard_binary_impl(first, last, value, comp);
        }
        else if constexpr (k == policy_kind::galloping)
        {
            // TODO: you decide what "galloping" maps to internally
            // return hybrid_search::detail::lower_bound_gallop_impl(first, last, value, comp);
            return hybrid_search::detail::lower_bound_standard_binary_impl(first, last, value, comp); // temporary fallback
        }
        else if constexpr (k == policy_kind::hybrid)
        {
            return hybrid_search::detail::lower_bound_hybrid_impl(first, last, value, comp);
        }
        else
        {
            static_assert([] { return false; }(), "Unknown policy");
        }
    }


    template <class Policy>
    template <class T, class V>
    T* searcher<Policy>::lower_bound(std::span<T> s, const V& value)
    {
        return lower_bound(s, value, std::less<>{});
    }

    template <class Policy>
    template <class T, class V, class Comp>
    requires lower_bound_comparator<Comp, T*, V>
    T* searcher<Policy>::lower_bound(std::span<T> s, const V& value, Comp comp)
    {
        return lower_bound(s.data(), s.data() + s.size(), value, comp);
    }

    template <class Policy>
    template <class T, class V>
    const T* searcher<Policy>::lower_bound(std::span<const T> s, const V& value)
    {
        return lower_bound(s, value, std::less<>{});
    }

    template <class Policy>
    template <class T, class V, class Comp>
    requires lower_bound_comparator<Comp, const T*, V>
    const T* searcher<Policy>::lower_bound(std::span<const T> s, const V& value, Comp comp)
    {
        return lower_bound(s.data(), s.data() + s.size(), value, comp);
    }


    template <class Policy>
    template <class T, class V>
    T* searcher<Policy>::lower_bound(T* first, T* last, const V& value)
    {
        return lower_bound(first, last, value, std::less<>{});
    }

    template <class Policy>
    template <class T, class V, class Comp>
    requires lower_bound_comparator<Comp, T*, V>
    T* searcher<Policy>::lower_bound(T* first, T* last, const V& value, Comp comp)
    {
        constexpr auto k = hybrid_search::policy::traits::policy_traits<Policy>::kind;

        if constexpr (k == policy_kind::standard_binary)
        {
            return hybrid_search::detail::lower_bound_standard_binary_impl(first, last, value, comp);
        }
        else if constexpr (k == policy_kind::galloping)
        {
            // TODO: you decide what "galloping" maps to internally
            // return hybrid_search::detail::lower_bound_gallop_impl(first, last, value, comp);
            return hybrid_search::detail::lower_bound_standard_binary_impl(first, last, value, comp); // temporary fallback
        }
        else if constexpr (k == policy_kind::hybrid)
        {
            return hybrid_search::detail::lower_bound_hybrid_impl(first, last, value, comp);
        }
        else
        {
            static_assert([] { return false; }(), "Unknown policy");
        }
    }

    template <class Policy>
    template <class T, class V>
    const T* searcher<Policy>::lower_bound(const T* first, const T* last, const V& value)
    {
        return lower_bound(first, last, value, std::less<>{});
    }

    template <class Policy>
    template <class T, class V, class Comp>
    requires lower_bound_comparator<Comp, const T*, V>
    const T* searcher<Policy>::lower_bound(const T* first, const T* last, const V& value, Comp comp)
    {
        constexpr auto k = hybrid_search::policy::traits::policy_traits<Policy>::kind;

        if constexpr (k == policy_kind::standard_binary)
        {
            return hybrid_search::detail::lower_bound_standard_binary_impl(first, last, value, comp);
        }
        else if constexpr (k == policy_kind::galloping)
        {
            // TODO: you decide what "galloping" maps to internally
            // return hybrid_search::detail::lower_bound_gallop_impl(first, last, value, comp);
            return hybrid_search::detail::lower_bound_standard_binary_impl(first, last, value, comp); // temporary fallback
        }
        else if constexpr (k == policy_kind::hybrid)
        {
            return hybrid_search::detail::lower_bound_hybrid_impl(first, last, value, comp);
        }
        else
        {
            static_assert([] { return false; }(), "Unknown policy");
        }
    }

}
