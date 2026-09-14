# Boundcraft

A header-only C++23 `lower_bound` / `upper_bound` where you pick the search
strategy as a template parameter. Same rules as the std versions (partitioned
range, same comparator conventions), same overload shapes, plus `std::span` and
raw pointer overloads. Everything is `constexpr`.

The original idea was to speed up the tail of a binary search with SIMD. That
part hasn't happened yet — what's here is the plain scalar version, which turned
out to be worth measuring on its own.

```cpp
#include <boundcraft/boundcraft.hpp>

namespace bp = boundcraft::policy;

std::vector<int> v = {1, 3, 3, 5, 7};
boundcraft::searcher<bp::hybrid<16>> s;

auto lo = s.lower_bound(v.begin(), v.end(), 3);  // v.begin() + 1
auto hi = s.upper_bound(v.begin(), v.end(), 3);  // v.begin() + 3
int* p  = s.lower_bound(std::span{v}, 5);        // &v[3]
```

## Using it

You need a C++23 compiler (GCC 13+, Clang 17+, MSVC 19.36+). Either copy
`include/` into your project or pull it in with CMake:

```cmake
include(FetchContent)
FetchContent_Declare(boundcraft
  GIT_REPOSITORY https://github.com/CharlesAdewumi2004/Boundcraft.git
  GIT_TAG        main)
FetchContent_MakeAvailable(boundcraft)

target_link_libraries(your_target PRIVATE boundcraft::boundcraft)
```

Tests and benchmarks only build when Boundcraft is the top-level project, so
they won't get dragged into yours.

## The API

`boundcraft::searcher<Policy>` has no state; every member is `constexpr`,
`const` and `[[nodiscard]]`. You get:

- `lower_bound(first, last, value)` and `upper_bound(first, last, value)` —
  uses `std::less<>`.
- The same with a `comp` argument. For `lower_bound` it's called as
  `comp(elem, value)`, for `upper_bound` as `comp(value, elem)`, exactly like
  the standard. Only that one call shape is required, so a comparator that
  compares an `Elem` against an `int` key needs a single `operator()`.
- `lower_bound_strict` / `upper_bound_strict` — same thing but `comp` has to
  work for elem/elem, elem/key and key/elem, if you want the standard's full
  strict-weak-order requirement enforced.
- `std::span<T>` / `std::span<const T>` and `T*` / `const T*` overloads that
  return a pointer.

Works in constant expressions:

```cpp
constexpr int a[] = {1, 3, 5, 7};
static_assert(boundcraft::searcher<bp::standard_binary>{}.lower_bound(a, a + 4, 5) == a + 2);
```

## Policies

| Policy | What it does | Needs |
|---|---|---|
| `standard_binary` | Plain binary search, same probes as `std::`. | forward iterators |
| `hybrid<N>` | Binary search until `N` or fewer elements are left, then scan linearly. | forward iterators |
| `galloping<Search, Start>` | Probe at `Start`, double the step outwards until the answer is bracketed, then hand the bracket to `Search`. | random-access iterators (it `static_assert`s otherwise) |

Start points live in `boundcraft::policy::gallop`: `start_front`, `start_back`,
`start_middle`, `start_last_searched<I>` (a fixed index, clamped to the range).

If you're not sure, use `hybrid<16>`. `standard_binary` is there if you want
`std::` behaviour behind the same interface. Galloping is only worth it when
your keys land a small, fixed distance from the start point — merging two
sorted ranges, or feeding it a monotone stream of keys with
`start_last_searched`. For keys spread over the whole range it's slower than
binary search, see below.

## Numbers

A caveat first: these were measured on a laptop — an i9-12900HX under WSL2, no
core pinning, no fixed clock, with whatever else was open at the time. Not a
benchmarking machine. Read the table as a ranking, not as absolute nanoseconds.

The row to calibrate against is `standard_binary`. It compiles to the same
nine-instruction loop as libstdc++'s `std::lower_bound` (compare the two with
`g++ -O3 -S`), yet it measures 2–12 % faster than `std::` here. That gap is the
error bar for the whole table: a difference under about 10 % between any two
rows doesn't mean anything on this machine. It's also why the comparisons below
are against `standard_binary` rather than `std::` — same harness, same run,
same artifacts.

Setup: sorted `std::vector<int>` of unique values, 4096 queries generated up
front and cycled. *uniform* is 50/50 hits and misses over the whole array;
*near-front* / *near-back* draw keys from the first / last `n/16` elements.
Median of 10 repetitions with Google Benchmark's random interleaving on, so no
policy systematically gets the warm or the cold slot. GCC 15.2, libstdc++,
CMake Release (`-O3`). Standard deviation within a run was about 7 % of the
median, occasionally 14 %.

**`lower_bound`** — median ns per query, 10 interleaved runs, lower is better

| Policy | uniform 16K | uniform 4M | near-front 16K | near-front 4M | near-back 16K | near-back 4M |
|---|---:|---:|---:|---:|---:|---:|
| `std::` (libstdc++) | 59.1 | 234.4 | 40.2 | 101.1 | 42.9 | 103.9 |
| `standard_binary` | 54.9 | 223.6 | 38.6 | 98.0 | 37.6 | 94.1 |
| `hybrid<16>` | 50.2 | 228.1 | 31.6 | 97.1 | 33.4 | 95.2 |
| `hybrid<64>` | 53.0 | 240.4 | 33.2 | 98.9 | 34.7 | 92.9 |
| `galloping<standard_binary, start_front>` | 60.7 | 249.8 | 39.3 | 101.6 | 45.7 | 111.4 |
| `galloping<standard_binary, start_middle>` | 62.0 | 250.2 | 46.2 | 114.2 | 47.0 | 116.0 |
| `galloping<hybrid<16>, start_front>` | 53.9 | 240.4 | 34.7 | 101.1 | 39.1 | 111.3 |

**`upper_bound`** — median ns per query, 10 interleaved runs, lower is better

| Policy | uniform 16K | uniform 4M | near-front 16K | near-front 4M | near-back 16K | near-back 4M |
|---|---:|---:|---:|---:|---:|---:|
| `std::` (libstdc++) | 57.6 | 237.8 | 41.1 | 103.3 | 42.2 | 101.3 |
| `standard_binary` | 56.3 | 230.4 | 40.2 | 97.4 | 38.6 | 95.0 |
| `hybrid<16>` | 49.8 | 240.0 | 31.2 | 93.7 | 33.4 | 92.6 |
| `hybrid<64>` | 52.3 | 234.0 | 34.0 | 98.2 | 34.9 | 96.0 |
| `galloping<standard_binary, start_front>` | 60.3 | 249.1 | 40.0 | 100.8 | 48.3 | 116.4 |
| `galloping<standard_binary, start_middle>` | 59.7 | 242.3 | 46.6 | 114.4 | 45.6 | 115.8 |
| `galloping<hybrid<16>, start_front>` | 53.9 | 240.2 | 34.4 | 103.0 | 42.3 | 108.1 |

What holds up: `hybrid<16>` is 9–22 % faster than `standard_binary` at 16K in
every pattern, for both operations. The near-front and near-back wins clear
the error bar; the uniform ones sit at its edge. The reason is visible in the
assembly — GCC unrolls the ≤16-element tail into a straight chain of compares,
so the last four levels of the binary search (four data-dependent branches,
each guessed wrong about half the time) become one scan that mispredicts
roughly once. At 4M the two are level, 0.96–1.04: the cost there is a handful
of cache-missing probes at the top of the search, and nothing below them
matters.

Galloping is 0–25 % slower than `standard_binary` almost everywhere. Doubling
outwards to reach index `k` costs about log₂k probes, then binary searching the
bracket costs about log₂k more, so it only beats a plain log₂n search when
`k < √n` — the first ~2,000 elements of 4M, or ~128 of 16K. The "near-front"
region here is `n/16`, well past that. The one exception is
`galloping<hybrid<16>, start_front>` on near-front 16K, which lands within noise
of plain `hybrid<16>`: the start point is close enough that the gallop is short
and the hybrid tail does the work.

To rerun:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/bench/BM_bounds_compare --benchmark_filter='(uniform|front|back)/(16384|4194304)$' \
    --benchmark_repetitions=10 --benchmark_enable_random_interleaving=true \
    --benchmark_report_aggregates_only=true
```

## Tests

```sh
cmake -S . -B build
cmake --build build -j
ctest --test-dir build
```

Every policy is checked against `std::lower_bound` / `std::upper_bound`: edge
cases, random sorted data with duplicates, descending data with
`std::greater<>`, element-vs-key comparators, and every key against every array
size up to 40. The non-galloping ones also run on `std::forward_list`.

## License

MIT, see [LICENSE](LICENSE).
