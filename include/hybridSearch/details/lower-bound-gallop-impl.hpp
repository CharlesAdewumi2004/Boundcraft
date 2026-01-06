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

      const diff_t avail_left = start_point - first; // how many steps we can go left (>=0)

      diff_t low = 0;
      diff_t high = 1;

      while (high <= avail_left && !comp(*(start_point - high), value))
      {
         low = high;
         high *= 2;
      }

      diff_t upper = low;
      diff_t lower = (high > avail_left) ? avail_left : high; 

      first = start_point - lower; 
      last = start_point - upper + 1;
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
