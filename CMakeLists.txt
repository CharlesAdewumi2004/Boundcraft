cmake_minimum_required(VERSION 3.24)

project(boundcraft VERSION 0.1.0 LANGUAGES CXX)

# ---- Options ----
option(BOUNDCRAFT_BUILD_TESTS "Build boundcraft tests" ON)
option(BOUNDCRAFT_BUILD_BENCH  "Build boundcraft benchmarks" OFF)

# ---- Library target (header-only) ----
add_library(boundcraft INTERFACE)
add_library(boundcraft::boundcraft ALIAS boundcraft)

target_include_directories(boundcraft
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(boundcraft INTERFACE cxx_std_20)

if (MSVC)
  target_compile_options(boundcraft INTERFACE /W4)
else()
  target_compile_options(boundcraft INTERFACE -Wall -Wextra -Wpedantic)
endif()

# ---- Tests ----
if (BOUNDCRAFT_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# ---- Benchmarks ----
if (BOUNDCRAFT_BUILD_BENCH)
  add_subdirectory(bench)
endif()

# ---- Install / export ----
include(GNUInstallDirs)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS boundcraft EXPORT boundcraftTargets)

install(EXPORT boundcraftTargets
  FILE boundcraftTargets.cmake
  NAMESPACE boundcraft::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/boundcraft
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/boundcraftConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/boundcraftConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/boundcraftConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/boundcraft
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/boundcraftConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/boundcraftConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/boundcraft
)
