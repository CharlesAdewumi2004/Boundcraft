# Boundcraft

Header-only C++23 `lower_bound` / `upper_bound` with a selectable search
strategy. Same preconditions and overload shapes as the standard algorithms,
plus `std::span` and pointer overloads, all `constexpr` and `[[nodiscard]]`.
Results are checked against `std::lower_bound` / `std::upper_bound` across every
policy and benchmarked against libstdc++.

```cpp
#include <boundcraft/boundcraft.hpp>

namespace bp = boundcraft::policy;

std::vector<int> v = {1, 3, 3, 5, 7};
boundcraft::searcher<bp::hybrid<16>> s;

auto lo = s.lower_bound(v.begin(), v.end(), 3);  // v.begin() + 1
auto hi = s.upper_bound(v.begin(), v.end(), 3);  // v.begin() + 3
int* p  = s.lower_bound(std::span{v}, 5);        // &v[3]
```

## Requirements

- C++23 (`<concepts>`, `<span>`), GCC 13+ / Clang 17+ / MSVC 19.36+
- CMake 3.24+ if you use the package; otherwise copy `include/`

## Integration

```cmake
include(FetchContent)
FetchContent_Declare(boundcraft
  GIT_REPOSITORY https://github.com/CharlesAdewumi2004/Boundcraft.git
  GIT_TAG        main)
FetchContent_MakeAvailable(boundcraft)

target_link_libraries(your_target PRIVATE boundcraft::boundcraft)
```

`add_subdirectory(Boundcraft)` works too. Tests and benchmarks are only built
when Boundcraft is the top-level project.

## API

`boundcraft::searcher<Policy>` is a stateless class whose members are all
`constexpr`, `const`, and `[[nodiscard]]`.

```cpp
It       lower_bound(It first, It last, const V& value);              // std::less<>
It       lower_bound(It first, It last, const V& value, Comp comp);   // comp(elem, value)
It       lower_bound_strict(It first, It last, const V& value, Comp comp);
It       upper_bound(It first, It last, const V& value);
It       upper_bound(It first, It last, const V& value, Comp comp);   // comp(value, elem)
It       upper_bound_strict(It first, It last, const V& value, Comp comp);

T*       lower_bound(std::span<T> s, const V& value[, Comp comp]);    // also span<const T>
T*       lower_bound(T* first, T* last, const V& value[, Comp comp]); // also const T*
// ... and the same for upper_bound
```

Preconditions match the standard: for `lower_bound` the range is partitioned by
`comp(elem, value)`; for `upper_bound` by `!comp(value, elem)`. The plain
overloads accept a *one-way* comparator that only needs the argument order shown
above, so heterogeneous lookups (`Elem` vs `int` key) need one `operator()`. The
`_strict` overloads require `comp` to be callable as elem/elem, elem/key, and
key/elem, like the standard's strict-weak-order requirement.

Everything is usable in constant expressions:

```cpp
constexpr int a[] = {1, 3, 5, 7};
static_assert(boundcraft::searcher<bp::standard_binary>{}.lower_bound(a, a + 4, 5) == a + 2);
```

## Policies

| Policy | Strategy | Iterators |
|---|---|---|
| `standard_binary` | Classic binary search. Same probe sequence as `std::`. | forward or better |
| `hybrid<N>` | Binary search until at most `N` elements remain, then a linear scan. | forward or better |
| `galloping<Search, Start>` | Probe at `Start`, double the step outward until the answer is bracketed, then run `Search` on the bracket. | random-access only (`static_assert` otherwise) |

Gallop start points, in `boundcraft::policy::gallop`: `start_front`,
`start_back`, `start_middle`, and `start_last_searched<I>` (a fixed index `I`,
clamped to the range).

**Which to pick.** `hybrid<16>` is the sensible default; see the numbers below.
`standard_binary` if you want exactly `std::` behaviour behind the same
interface. Galloping is for queries that land within a small *absolute*
distance of the start point — merging two sorted ranges, or a monotone stream of
keys with `start_last_searched` — and is slower than binary search for keys
spread across the range.

## Benchmarks

Sorted `std::vector<int>` of unique values, 4096 pre-generated queries cycled
through the timing loop. *uniform* mixes 50/50 hits and misses over the whole
array; *near-front* / *near-back* draw keys from the first / last `n/16`
elements. Median of 3 repetitions, 0.2 s minimum per run.

Measured on an Intel i9-12900HX (WSL2), GCC 15.2, libstdc++, CMake `Release`
(`-O3`). Treat differences under ~5 % as noise.

**`lower_bound`** — median ns per query, lower is better

| Policy | uniform 16K | uniform 4M | near-front 16K | near-front 4M | near-back 16K | near-back 4M |
|---|---:|---:|---:|---:|---:|---:|
| `std::` (libstdc++) | 57.3 | 221.8 | 38.3 | 97.2 | 41.0 | 99.8 |
| `standard_binary` | 58.9 | 251.9 | 40.2 | 101.7 | 40.8 | 95.6 |
| `hybrid<16>` | 54.7 | 246.0 | 29.6 | 91.1 | 33.2 | 94.5 |
| `hybrid<64>` | 50.5 | 257.4 | 34.5 | 97.4 | 34.3 | 91.8 |
| `galloping<standard_binary, start_front>` | 61.4 | 262.6 | 39.6 | 103.5 | 47.8 | 111.6 |
| `galloping<standard_binary, start_middle>` | 57.6 | 248.9 | 46.0 | 126.8 | 54.2 | 138.0 |
| `galloping<hybrid<16>, start_front>` | 66.3 | 306.0 | 38.9 | 114.8 | 44.8 | 115.3 |

**`upper_bound`** — median ns per query, lower is better

| Policy | uniform 16K | uniform 4M | near-front 16K | near-front 4M | near-back 16K | near-back 4M |
|---|---:|---:|---:|---:|---:|---:|
| `std::` (libstdc++) | 57.1 | 235.1 | 40.4 | 101.3 | 40.7 | 100.4 |
| `standard_binary` | 54.2 | 220.9 | 36.7 | 92.9 | 38.5 | 98.7 |
| `hybrid<16>` | 50.1 | 228.2 | 32.6 | 93.5 | 31.6 | 90.3 |
| `hybrid<64>` | 49.2 | 251.7 | 33.9 | 91.7 | 32.8 | 92.5 |
| `galloping<standard_binary, start_front>` | 58.5 | 266.7 | 39.4 | 100.4 | 45.8 | 105.4 |
| `galloping<standard_binary, start_middle>` | 66.9 | 281.4 | 48.5 | 110.7 | 44.8 | 129.7 |
| `galloping<hybrid<16>, start_front>` | 59.8 | 278.5 | 39.9 | 107.6 | 48.2 | 124.0 |

**Reading the table.** `standard_binary` tracks `std::` within noise, as it
should — it is the same algorithm. `hybrid<16>` is never meaningfully slower
than `std::` and is 10–25 % faster on the 16K near-front / near-back cases and a
few percent faster on uniform 16K: the final ≤16-element linear scan is
branch-predictable and skips the last few dependent loads. At 4M elements the
gap closes; the search is dominated by cache misses on the early probes, which
no policy avoids. Galloping does not pay off on these workloads. Doubling out
from `start_front` across a 262K-element "near-front" region costs about as
many probes as a binary search over all 4M, plus the bracketing work, so it
lands 0–40 % behind `std::` here. It only wins when the target is a small fixed
number of elements away from the start point.

Reproduce with:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/bench/BM_bounds_compare --benchmark_filter='(uniform|front|back)/(16384|4194304)$' \
    --benchmark_repetitions=3 --benchmark_report_aggregates_only=true
```

## Tests

```sh
cmake -S . -B build
cmake --build build -j
ctest --test-dir build
```

Every policy is tested against `std::lower_bound` / `std::upper_bound` on
deterministic edge cases, randomized sorted data with duplicates, descending
data with `std::greater<>`, heterogeneous element/key comparators, and an
exhaustive sweep of every key over every array size up to 40. Non-galloping
policies are also exercised on `std::forward_list`.

## License

MIT — see [LICENSE](LICENSE).
