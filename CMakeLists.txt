cmake_minimum_required(VERSION 3.24)

project(boundcraft VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# =========================================
# Options
# - Default OFF when used as a dependency
# - Default ON when Boundcraft is top-level
# =========================================
option(BOUNDCRAFT_BUILD_TESTS "Build boundcraft tests" OFF)
option(BOUNDCRAFT_BUILD_BENCH "Build boundcraft benchmarks" OFF)

if (PROJECT_IS_TOP_LEVEL)
  set(BOUNDCRAFT_BUILD_TESTS ON CACHE BOOL "Build boundcraft tests" FORCE)
  set(BOUNDCRAFT_BUILD_BENCH ON CACHE BOOL "Build boundcraft benchmarks" FORCE)
endif()

# =========================
# Library target (header-only)
# =========================
add_library(boundcraft INTERFACE)
add_library(boundcraft::boundcraft ALIAS boundcraft)

target_include_directories(boundcraft
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(boundcraft INTERFACE cxx_std_23)

if (MSVC)
  target_compile_options(boundcraft INTERFACE /W4)
else()
  target_compile_options(boundcraft INTERFACE -Wall -Wextra -Wpedantic)
endif()

# ======
# Tests
# ======
if (BOUNDCRAFT_BUILD_TESTS)
  include(CTest) # enables testing + adds 'test' target when top-level
  if (EXISTS "${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
  else()
    message(WARNING "BOUNDCRAFT_BUILD_TESTS=ON but ${PROJECT_SOURCE_DIR}/tests/ was not found. Skipping tests.")
  endif()
endif()

# ==========
# Benchmarks
# ==========
if (BOUNDCRAFT_BUILD_BENCH)
  if (EXISTS "${PROJECT_SOURCE_DIR}/bench/CMakeLists.txt")
    add_subdirectory(bench)
  else()
    message(WARNING "BOUNDCRAFT_BUILD_BENCH=ON but ${PROJECT_SOURCE_DIR}/bench/ was not found. Skipping benchmarks.")
  endif()
endif()

# ==================
# Install / Export
# ==================
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(TARGETS boundcraft
        EXPORT boundcraftTargets)

install(EXPORT boundcraftTargets
  FILE boundcraftTargets.cmake
  NAMESPACE boundcraft::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/boundcraft"
)

write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/boundcraftConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/boundcraftConfig.cmake.in"
  "${PROJECT_BINARY_DIR}/boundcraftConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/boundcraft"
)

install(FILES
  "${PROJECT_BINARY_DIR}/boundcraftConfig.cmake"
  "${PROJECT_BINARY_DIR}/boundcraftConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/boundcraft"
)
