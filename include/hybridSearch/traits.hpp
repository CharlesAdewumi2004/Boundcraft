#pragma once

#include "policy.hpp"

enum class policy_kind
{
    standard_binary,
    galloping,
    hybrid
};

namespace hybrid_search::policy::traits
{

    template <class Policy>
    struct policy_traits;

    template <>
    struct policy_traits<standard_binary>
    {
        static constexpr policy_kind kind = policy_kind::standard_binary;
    };

    template <class Search_Policy, class Gallop_Start>
    struct policy_traits<galloping<Search_Policy, Gallop_Start>>
    {
        static constexpr policy_kind kind = policy_kind::galloping;
        using policy_search_policy = Search_Policy;
        using gallop_start = Gallop_Start; 
    };

    template <std::size_t T>
    struct policy_traits<hybrid<T>>
    {
        static constexpr policy_kind kind = policy_kind::hybrid;
        static constexpr std::size_t threshold = T;
    };
};


namespace hybrid_search::policy::gallop::traits {

template <class T>
struct is_gallop_start_policy : std::false_type {};

template <>
struct is_gallop_start_policy<start_front> : std::true_type {};

template <>
struct is_gallop_start_policy<start_back> : std::true_type {};

template <>
struct is_gallop_start_policy<start_middle> : std::true_type {};

template <std::size_t P>
struct is_gallop_start_policy<start_last_searched<P>> : std::true_type {};

template <class T>
inline constexpr bool is_gallop_start_policy_v = is_gallop_start_policy<T>::value;

enum class kind { front, back, middle, last_searched };

template <class T>
struct start_kind; 

template <>
struct start_kind<start_front> : std::integral_constant<kind, kind::front> {};

template <>
struct start_kind<start_back> : std::integral_constant<kind, kind::back> {};

template <>
struct start_kind<start_middle> : std::integral_constant<kind, kind::middle> {};

template <std::size_t P>
struct start_kind<start_last_searched<P>> : std::integral_constant<kind, kind::last_searched> {};

template <class T>
inline constexpr kind start_kind_v = start_kind<T>::value;

template <class T>
struct start_point : std::integral_constant<std::size_t, 0> {}; // default 0

template <std::size_t P>
struct start_point<start_last_searched<P>> : std::integral_constant<std::size_t, P> {};

template <class T>
inline constexpr std::size_t start_point_v = start_point<T>::value;

} 

