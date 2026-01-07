#include <gtest/gtest.h>
#include "../include/Boundcraft/searcher.hpp"
#include <vector>


TEST(Smoke, GTestWiresUp) {
    EXPECT_EQ(1 + 1, 2);
    std::vector<int> a(50);
    int *p1 = &a[0];
    int *p2 = &a[49];
    auto s = hybrid_search::searcher<hybrid_search::policy::hybrid<32>>();
    s.lower_bound(p1, p2, 5);
}


