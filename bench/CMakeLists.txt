include(FetchContent)

# Google Benchmark can optionally build its own tests; we don't need them
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
)

FetchContent_MakeAvailable(benchmark)

add_executable(BM_bounds_compare BM_bounds_compare.cpp)

target_link_libraries(BM_bounds_compare
  PRIVATE
    boundcraft::boundcraft
    benchmark::benchmark
)

target_compile_features(BM_bounds_compare PRIVATE cxx_std_23)


