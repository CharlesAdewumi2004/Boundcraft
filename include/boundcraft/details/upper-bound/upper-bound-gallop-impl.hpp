#pragma once

#include <boundcraft/details/upper-bound/upper-bound-hybrid-impl.hpp>
#include <boundcraft/details/upper-bound/upper-bound-standard-impl.hpp>
#include <boundcraft/details/upper-bound/upper-bound-util.hpp>

namespace boundcraft::detail
{



    template <class Search_Policy, class Gallop_Start, class It, class V, class Comp>
        requires(!std::random_access_iterator<It>)
    constexpr It upper_bound_gallop_impl(It, It, const V &, Comp)
    {
        static_assert(always_false_v<It>,
                      "Boundcraft: galloping upper_bound requires RANDOM-ACCESS iterators "
                      "(e.g. std::vector, std::span, pointers). "
                      "Use standard_binary/hybrid for forward iterators (e.g. std::forward_list).");
        return It{};
    }

    template <class Search_Policy, class Gallop_Start, class It, class V, class Comp>
        requires std::random_access_iterator<It>
    constexpr It upper_bound_gallop_impl(It first, It last, const V &value, Comp comp)
    {
        if (first == last)
        {
            return first;
        }

        static_assert(is_random_access_iter_v<It>,
                      "upper_bound_gallop_impl requires random-access iterators (uses + and -).");

        namespace g = boundcraft::policy::gallop::traits;
        static_assert(g::is_gallop_start_policy_v<Gallop_Start>,
                      "Gallop_Start must be a boundcraft::policy::gallop start policy");

        using diff_t = typename std::iterator_traits<It>::difference_type;

        It start_point = first;

        if constexpr (g::start_kind_v<Gallop_Start> == g::kind::front)
        {
            start_point = first;
        }
        else if constexpr (g::start_kind_v<Gallop_Start> == g::kind::back)
        {
            start_point = last - 1;
        }
        else if constexpr (g::start_kind_v<Gallop_Start> == g::kind::middle)
        {
            start_point = first + (last - first) / 2;
        }
        else if constexpr (g::start_kind_v<Gallop_Start> == g::kind::last_searched)
        {
            constexpr std::size_t p = g::start_point_v<Gallop_Start>;

            const diff_t n = last - first;
            const diff_t clamped =
                (p >= static_cast<std::size_t>(n)) ? (n - 1) : static_cast<diff_t>(p);

            start_point = first + clamped;
        }
        else
        {
            static_assert([]
                          { return false; }(), "Unknown gallop policy");
        }

        It lo = first;
        It hi = last;

        if (!comp(value, *start_point))
        {
            if (start_point == last - 1)
            {
                return last;
            }
            upper_bound_expand_right(lo, hi, start_point, value, comp);
        }
        else
        {
            if (start_point == first)
            {
                return first;
            }
            upper_bound_expand_left(lo, hi, start_point, value, comp);
        }

        using ptraits = boundcraft::policy::traits::policy_traits<Search_Policy>;
        constexpr auto kind = ptraits::kind;

        if constexpr (kind == policy_kind::standard_binary)
        {
            return boundcraft::detail::upper_bound_standard_binary_impl(lo, hi, value, comp);
        }
        else if constexpr (kind == policy_kind::hybrid)
        {
            constexpr std::size_t threshold = ptraits::threshold;
            return boundcraft::detail::upper_bound_hybrid_impl(threshold, lo, hi, value, comp);
        }
        else
        {
            static_assert([]
                          { return false; }(), "Unknown policy");
        }
    }
}

