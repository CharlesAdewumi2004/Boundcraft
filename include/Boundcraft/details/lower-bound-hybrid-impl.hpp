#pragma once

#include "util.hpp"

namespace boundcraft::detail
{

    

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
        first = lower_bound_linear_scan(first, count, value, comp);
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
