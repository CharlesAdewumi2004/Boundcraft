include(FetchContent)

# Google Benchmark can optionally build its own tests; we don't need them
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
)

FetchContent_MakeAvailable(benchmark)

add_executable(boundcraft_bench
  bm_lower_bound.cpp
)

target_link_libraries(boundcraft_bench
  PRIVATE
    boundcraft::boundcraft
    benchmark::benchmark
    benchmark::benchmark_main
)

target_compile_features(boundcraft_bench PRIVATE cxx_std_20)
