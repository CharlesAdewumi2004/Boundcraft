#pragma once

#include <cstddef>

namespace boundcraft::policy
{
    struct standard_binary final
    {
    };

    template <std::size_t Threshold>
    struct hybrid final
    {
        static constexpr std::size_t threshold = Threshold;
    };

    template <class Search_Policy, class Gallop_Start>
    struct galloping final
    {
        using search_policy = Search_Policy;
        using gallop_start = Gallop_Start;
    };

}

namespace boundcraft::policy::gallop
{
    struct start_front final
    {
    };
    struct start_back final
    {
    };
    struct start_middle final
    {
    };
    
    template<std::size_t Start_Point>
    struct start_last_searched final
    {
        size_t start_point = Start_Point;
    };
}