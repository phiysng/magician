cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)

project(magician VERSION 0.0.1)

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

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

add_library(delegate INTERFACE magician/delegate.hpp)
target_include_directories(delegate INTERFACE .)
target_compile_features(delegate INTERFACE cxx_std_17)

add_library(traits INTERFACE magician/traits.hpp)
target_include_directories(traits INTERFACE .)
target_compile_features(traits INTERFACE cxx_std_17)
target_link_libraries(traits INTERFACE nlohmann_json::nlohmann_json)

add_library(static-reflection INTERFACE magician/static_reflection.hpp)
target_include_directories(static-reflection INTERFACE .)
target_compile_features(static-reflection INTERFACE cxx_std_17)
target_link_libraries(static-reflection INTERFACE nlohmann_json::nlohmann_json)

add_executable(test-traits tests/test_traits.cpp)
target_link_libraries(test-traits PRIVATE GTest::gtest_main traits)

add_executable(test-delegate tests/test_delegate.cpp)
target_link_libraries(test-delegate PRIVATE GTest::gtest_main nlohmann_json::nlohmann_json delegate)

add_executable(test-static-reflection tests/test_static_reflection.cpp)
target_link_libraries(test-static-reflection PRIVATE GTest::gtest_main nlohmann_json::nlohmann_json static-reflection)

include(GoogleTest)
gtest_discover_tests(test-traits)
gtest_discover_tests(test-delegate)
gtest_discover_tests(test-static-reflection)