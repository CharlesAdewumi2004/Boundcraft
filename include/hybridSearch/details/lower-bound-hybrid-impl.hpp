#pragma once

#include <cstddef>
#include <iterator>

#include "lower-bound-standard-impl.hpp"

namespace hybrid_search::detail
{

    template <class It, class V, class Comp>
    inline It lower_bound_linear_scan(It first, typename std::iterator_traits<It>::difference_type count, const V &value, Comp comp)
    {
        while (count > 0)
        {
            if (!comp(*first, value))
            {
                break;
            }
            ++first;
            --count;
        }
        return first;
    }

    /* ============================================================
       RANDOM ACCESS ITERATORS
       ============================================================ */

    template <random_it RandomIt, class V, class Comp>
    inline RandomIt lower_bound_hybrid_impl(size_t range, RandomIt first, RandomIt last, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

        diff_t count = last - first;
        while (count > static_cast<diff_t>(range))
        {
            lower_bound_probe_ra(first, count, value, comp);
        }
        lower_bound_linear_scan(first, count, value, comp);
        return first;
    }

    /* ============================================================
       FORWARD ITERATORS
       ============================================================ */

    template <forward_not_random_it RandomIt, class V, class Comp>
    inline RandomIt lower_bound_hybrid_impl(size_t range, RandomIt first, RandomIt last, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

        diff_t count = std::distance(first, last);
        while (count > range)
        {
            lower_bound_probe_fw(first, count, value, comp);
        }
        first = lower_bound_linear_scan(first, count, value, comp);
        return first;
    }

}
