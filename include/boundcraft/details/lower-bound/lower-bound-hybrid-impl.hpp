#pragma once

#include <boundcraft/details/lower-bound/lower-bound-util.hpp>

namespace boundcraft::detail
{

    template <random_it RandomIt, class V, class Comp>
    inline constexpr RandomIt lower_bound_hybrid_impl(size_t range, RandomIt first, RandomIt last, const V &value, Comp comp)
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

    template <forward_not_random_it RandomIt, class V, class Comp>
    inline constexpr RandomIt lower_bound_hybrid_impl(size_t range, RandomIt first, RandomIt last, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

        diff_t count = std::distance(first, last);
        while (count > static_cast<diff_t>(range))
        {
            lower_bound_probe_fw(first, count, value, comp);
        }
        first = lower_bound_linear_scan(first, count, value, comp);
        return first;
    }

}
