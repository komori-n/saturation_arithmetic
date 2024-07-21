cmake_minimum_required(VERSION 3.14)

project(komori_saturation_arithmetic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(komori_saturation_arithmetic INTERFACE)
target_include_directories(komori_saturation_arithmetic INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  test_komori_saturation_arithmetic
  tests/saturation_arithmetic_test.cpp
)
target_link_libraries(
  test_komori_saturation_arithmetic
  komori_saturation_arithmetic
  GTest::gtest_main
)
if(NOT MSVC)
  target_compile_options(test_komori_saturation_arithmetic PRIVATE -Wall -Wextra -Wno-narrowing)
endif()

include(GoogleTest)
gtest_discover_tests(test_komori_saturation_arithmetic)
