#pragma once

#include <cstddef>
#include <iterator>

#include "../include/hybridSearch/policy.hpp"

namespace hybrid_search::detail
{

   template <class RandomIt, class V, class Comp>
   void expand_right(RandomIt &first, RandomIt &last, RandomIt start_point, const V &value, Comp comp)
   {
      using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

      diff_t low = 0;
      diff_t high = 1;

      while (start_point + high < last && comp(*(start_point + high), value))
      {
         low = high;
         high *= 2;
      }

      if (start_point + high < last)
      {
         last = start_point + high;
      }
      first = start_point + (low + 1);
   }

   template <class RandomIt, class V, class Comp>
   void expand_left(RandomIt &first, RandomIt &last, RandomIt start_point, const V &value, Comp comp)
   {
      using diff_t = typename std::iterator_traits<RandomIt>::difference_type;

      diff_t low = 0;
      diff_t high = 1;

      while (start_point - high >= first && comp(value, *(start_point - high)))
      {
         low = high;
         high *= 2;
      }

      if (start_point - high >= high)
      {
         high = start_point - high;
      }
      last = start_point - (low + 1);
   }

   template <class Search_Policy, class Gallop_Start, class It, class V, class Comp>
   It lower_bound_gallop_impl(It first, It last, const V &value, Comp comp)
   {
      if constexpr (std::is_same_v(hybrid_search::policy::gallop::start_front, Gallop_Start))
      {
      }
      else if constexpr (std::is_same_v(hybrid_search::policy::gallop::start_back, Gallop_Start))
      {
      }
      else if constexpr (std::is_same_v(hybrid_search::policy::gallop::start_middle, Gallop_Start))
      {
      }
      else if constexpr (std::is_same_v(hybrid_search::policy::gallop::start_last_searched, Gallop_Start))
      {
      }
      else
      {
         static_assert([]{ return false; }(), "Unknown gallop policy");
      }
   }
}
