#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

#include <hybridSearch/policy.hpp>
#include <hybridSearch/traits.hpp>

#include "lower-bound-hybrid-impl.hpp"
#include "lower-bound-standard-impl.hpp"

namespace hybrid_search::detail
{
    template <class RandomIt, class V, class Comp>
    inline void expand_right(RandomIt& first, RandomIt& last, RandomIt start_point, const V& value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

        diff_t low = 0;
        diff_t high = 1;

        while (start_point + high < last && comp(*(start_point + high), value)) {
            low = high;
            high *= 2;
        }

        if (start_point + high < last) {
            last = start_point + high;
        }
        first = start_point + (low + 1);
    }

    template <class RandomIt, class V, class Comp>
    inline void expand_left(RandomIt& first, RandomIt& last, RandomIt start_point, const V& value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

        const diff_t avail_left = start_point - first;

        diff_t low = 0;
        diff_t high = 1;

        while (high <= avail_left && !comp(*(start_point - high), value)) {
            low = high;
            high *= 2;
        }

        const diff_t upper = low;
        const diff_t lower = (high > avail_left) ? avail_left : high;

        first = start_point - lower;
        last  = start_point - upper + 1;
    }

    template <class It>
    constexpr bool is_random_access_iter_v =
        std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<It>::iterator_category>;

    template <class Search_Policy, class Gallop_Start, class It, class V, class Comp>
    It lower_bound_gallop_impl(It first, It last, const V& value, Comp comp)
    {
        if (first == last) {
            return first;
        }

        static_assert(is_random_access_iter_v<It>,
                      "lower_bound_gallop_impl requires random-access iterators (uses + and -).");

        namespace g = hybrid_search::policy::gallop::traits;
        static_assert(g::is_gallop_start_policy_v<Gallop_Start>,
                      "Gallop_Start must be a hybrid_search::policy::gallop start policy");

        using diff_t = typename std::iterator_traits<It>::difference_type;

        It start_point = first;

        if constexpr (g::start_kind_v<Gallop_Start> == g::kind::front) {
            start_point = first;
        } else if constexpr (g::start_kind_v<Gallop_Start> == g::kind::back) {
            start_point = last - 1;
        } else if constexpr (g::start_kind_v<Gallop_Start> == g::kind::middle) {
            start_point = first + (last - first) / 2;
        } else if constexpr (g::start_kind_v<Gallop_Start> == g::kind::last_searched) {
            constexpr std::size_t p = g::start_point_v<Gallop_Start>;

            const diff_t n = last - first;
            const diff_t clamped =
                (p >= static_cast<std::size_t>(n)) ? (n - 1) : static_cast<diff_t>(p);

            start_point = first + clamped;
        } else {
            static_assert([] { return false; }(), "Unknown gallop policy");
        }

        It lo = first;
        It hi = last;

        if (comp(*start_point, value)) {
            if (start_point == last - 1) {
                return last;
            }
            expand_right(lo, hi, start_point, value, comp);
        } else {
            if (start_point == first) {
                return first;
            }
            expand_left(lo, hi, start_point, value, comp);
        }

        using ptraits = hybrid_search::policy::traits::policy_traits<Search_Policy>;
        constexpr auto kind = ptraits::kind;

        if constexpr (kind == policy_kind::standard_binary) {
            return hybrid_search::detail::lower_bound_standard_binary_impl(lo, hi, value, comp);
        } else if constexpr (kind == policy_kind::hybrid) {
            constexpr std::size_t threshold = ptraits::threshold;
            return hybrid_search::detail::lower_bound_hybrid_impl(threshold, lo, hi, value, comp);
        } else {
            static_assert([] { return false; }(), "Unknown policy");
        }
    }

} 
