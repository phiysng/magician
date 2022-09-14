project(magician)
cmake_minimum_required(VERSION 3.16)

enable_testing()
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(delegate INTERFACE delegate.hpp)
target_compile_features(delegate INTERFACE cxx_std_17)

add_library(traits INTERFACE traits.hpp)
target_compile_features(traits INTERFACE cxx_std_17)

add_executable(test-traits test_traits.cpp)
target_link_libraries(test-traits PRIVATE GTest::gtest_main traits)

include(GoogleTest)
gtest_discover_tests(test-traits)