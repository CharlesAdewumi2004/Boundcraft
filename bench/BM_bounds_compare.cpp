#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include <boundcraft/boundcraft.hpp>

namespace {

std::vector<int> make_sorted_unique(std::size_t n, int start = 0, int step = 2) {
    std::vector<int> v;
    v.reserve(n);
    int x = start;
    for (std::size_t i = 0; i < n; ++i, x += step) v.push_back(x);
    return v;
}

enum class QueryPattern { UniformRandom, MostlyHits, MostlyMisses, NearFront, NearBack };

int make_query(std::mt19937& rng, const std::vector<int>& data, QueryPattern pat) {
    const std::size_t n = data.size();
    if (n == 0) return 0;

    std::uniform_int_distribution<std::size_t> idx_dist(0, n - 1);
    std::uniform_int_distribution<int> coin(0, 99);

    auto pick_hit = [&] { return data[idx_dist(rng)]; };
    auto pick_miss = [&] {
        std::uniform_int_distribution<int> miss_dist(data.front(), data.back());
        int x = miss_dist(rng);
        return (x % 2 == 0) ? (x + 1) : x; // odd values never occur with step=2
    };
    auto near = [&](std::size_t lo, std::size_t hi) {
        std::uniform_int_distribution<std::size_t> d(lo, hi);
        std::uniform_int_distribution<int> jitter(-3, 3);
        return data[d(rng)] + jitter(rng);
    };

    switch (pat) {
        case QueryPattern::UniformRandom: return (coin(rng) < 50) ? pick_hit() : pick_miss();
        case QueryPattern::MostlyHits:    return (coin(rng) < 90) ? pick_hit() : pick_miss();
        case QueryPattern::MostlyMisses:  return (coin(rng) < 90) ? pick_miss() : pick_hit();
        case QueryPattern::NearFront:     return near(0, std::max<std::size_t>(1, n / 16) - 1);
        case QueryPattern::NearBack:      return near((n > 1) ? n - std::max<std::size_t>(1, n / 16) : 0, n - 1);
    }
    return data[idx_dist(rng)];
}

template <class Search>
void run_bench(benchmark::State& state, QueryPattern pat, Search search) {
    const auto n = static_cast<std::size_t>(state.range(0));
    const auto data = make_sorted_unique(n);

    std::mt19937 rng(123456u);
    std::vector<int> queries(4096);
    for (auto& q : queries) q = make_query(rng, data, pat);

    std::size_t qi = 0;
    std::size_t sink = 0;
    for (auto _ : state) {
        const int key = queries[qi++ & (queries.size() - 1)];
        auto it = search(data.cbegin(), data.cend(), key);
        sink += static_cast<std::size_t>(it - data.cbegin());
        benchmark::DoNotOptimize(sink);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}

template <class Search>
void register_all(const std::string& name, Search search) {
    static constexpr std::pair<const char*, QueryPattern> patterns[] = {
        {"uniform", QueryPattern::UniformRandom},
        {"hits",    QueryPattern::MostlyHits},
        {"misses",  QueryPattern::MostlyMisses},
        {"front",   QueryPattern::NearFront},
        {"back",    QueryPattern::NearBack},
    };
    for (auto [pname, pat] : patterns) {
        auto* b = benchmark::RegisterBenchmark(
            (name + "/" + pname).c_str(),
            [search, pat](benchmark::State& st) { run_bench(st, pat, search); });
        for (int shift : {10, 14, 18, 22}) b->Arg(1 << shift);
    }
}

struct std_lower { template <class It> It operator()(It f, It l, int k) const { return std::lower_bound(f, l, k); } };
struct std_upper { template <class It> It operator()(It f, It l, int k) const { return std::upper_bound(f, l, k); } };

template <class Policy>
struct bc_lower { template <class It> It operator()(It f, It l, int k) const { return boundcraft::searcher<Policy>{}.lower_bound(f, l, k); } };
template <class Policy>
struct bc_upper { template <class It> It operator()(It f, It l, int k) const { return boundcraft::searcher<Policy>{}.upper_bound(f, l, k); } };

template <class Policy>
void register_policy(const std::string& name) {
    register_all("bc/" + name + "/lower_bound", bc_lower<Policy>{});
    register_all("bc/" + name + "/upper_bound", bc_upper<Policy>{});
}

} // namespace

int main(int argc, char** argv) {
    namespace bp = boundcraft::policy;
    namespace bg = boundcraft::policy::gallop;

    register_all("std/lower_bound", std_lower{});
    register_all("std/upper_bound", std_upper{});

    register_policy<bp::standard_binary>("standard");
    register_policy<bp::hybrid<16>>("hybrid16");
    register_policy<bp::hybrid<64>>("hybrid64");
    register_policy<bp::galloping<bp::standard_binary, bg::start_front>>("gallop_std_front");
    register_policy<bp::galloping<bp::standard_binary, bg::start_middle>>("gallop_std_middle");
    register_policy<bp::galloping<bp::hybrid<16>, bg::start_front>>("gallop_hyb16_front");

    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}
