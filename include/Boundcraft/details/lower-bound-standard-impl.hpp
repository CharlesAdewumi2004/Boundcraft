#pragma once

#include "util.hpp"
namespace boundcraft::detail
{



    template <random_it RandomIt, class V, class Comp>
    inline RandomIt lower_bound_standard_binary_impl(
        RandomIt first, RandomIt last, const V &value, Comp comp)
    {
        auto count = last - first;
        while (count > 0)
        {
            lower_bound_probe_ra(first, count, value, comp);
        }
        return first;
    }

    template <forward_not_random_it ForwardIt, class V, class Comp>
    inline ForwardIt lower_bound_standard_binary_impl(
        ForwardIt first, ForwardIt last, const V &value, Comp comp)
    {
        auto count = std::distance(first, last);
        while (count > 0)
        {
            lower_bound_probe_fw(first, count, value, comp);
        }
        return first;
    }

}
