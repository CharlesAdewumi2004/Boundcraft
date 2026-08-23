#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

#include <boundcraft/details/util.hpp>
#include <boundcraft/policy.hpp>
#include <boundcraft/traits.hpp>



namespace boundcraft::detail
{
    

    template <random_it RandomIt, class V, class Comp>
    inline constexpr void lower_bound_probe_ra(
        RandomIt &first,
        std::iter_difference_t<RandomIt> &count,
        const V &value,
        Comp comp)
    {
        auto step = count / 2;
        RandomIt mid = first + step;

        if (comp(*mid, value))
        {
            first = mid + 1;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    template <forward_not_random_it ForwardIt, class V, class Comp>
    inline constexpr void lower_bound_probe_fw(ForwardIt &first, std::iter_difference_t<ForwardIt> &count, const V &value, Comp comp)
    {
        auto step = count / 2;
        ForwardIt mid = first;
        std::advance(mid, step);

        if (comp(*mid, value))
        {
            ++mid;
            first = mid;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    template <class It, class V, class Comp>
    [[nodiscard]] inline constexpr It lower_bound_linear_scan(It first, typename std::iterator_traits<It>::difference_type count, const V &value, Comp comp)
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

    template <class RandomIt, class V, class Comp>
    inline constexpr void lower_bound_expand_right(RandomIt &first, RandomIt &last, RandomIt start_point, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

        const diff_t avail_right = last - start_point;
        diff_t low = 0;
        diff_t high = 1;

        while (high < avail_right && comp(*(start_point + high), value))
        {
            low = high;
            high *= 2;
        }

        if (high < avail_right)
            last = start_point + high;

        first = start_point + (low + 1);
    }

    template <class RandomIt, class V, class Comp>
    inline constexpr void lower_bound_expand_left(RandomIt &first, RandomIt &last, RandomIt start_point, const V &value, Comp comp)
    {
        using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

        const diff_t avail_left = start_point - first;

        diff_t low = 0;
        diff_t high = 1;

        while (high <= avail_left && !comp(*(start_point - high), value))
        {
            low = high;
            high *= 2;
        }

        const diff_t upper = low;
        const diff_t lower = (high > avail_left) ? avail_left : high;

        first = start_point - lower;
        last = start_point - upper + 1;
    }

}