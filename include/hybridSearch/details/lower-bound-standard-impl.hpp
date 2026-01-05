#include <iterator>

namespace hybrid_search::detail
{

    template <class It>
    concept random_it = std::random_access_iterator<It>;

    template <class It>
    concept forward_not_random_it =
        std::forward_iterator<It> && (!std::random_access_iterator<It>);

    /* ===================== RANDOM ACCESS ===================== */

    template <random_it RandomIt, class V, class Comp>
    inline void lower_bound_probe_ra(
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

    /* ===================== FORWARD (NOT RANDOM) ===================== */

    template <forward_not_random_it ForwardIt, class V, class Comp>
    inline void lower_bound_probe_fw(
        ForwardIt &first,
        std::iter_difference_t<ForwardIt> &count,
        const V &value,
        Comp comp)
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
