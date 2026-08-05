#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>


namespace boundcraft::detail{

    template <class...>
    inline constexpr bool always_false_v = false;

    template <class It>
    constexpr bool is_random_access_iter_v = std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<It>::iterator_category>;

    template <class It>
    concept random_it = std::random_access_iterator<It>;

    template <class It>
    concept forward_not_random_it = std::forward_iterator<It> && (!std::random_access_iterator<It>);
}