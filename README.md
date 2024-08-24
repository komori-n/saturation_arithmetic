# saturation_arithmetic

[![pre-commit](https://github.com/komori-n/saturation_arithmetic/actions/workflows/pre-commit.yml/badge.svg)](https://github.com/komori-n/saturation_arithmetic/actions/workflows/pre-commit.yml) [![CMake on multiple platforms](https://github.com/komori-n/saturation_arithmetic/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/komori-n/saturation_arithmetic/actions/workflows/cmake-multi-platform.yml)

A header-only library dedicated to Saturation Arithmetic in C++14.

## Getting Started

### Bazel

Add the following lines in `MODULE.bazel`.

```starlark
bazel_dep(name = "komori_saturation_arithmetic", version="")
git_override(
    module_name = "komori_saturation_arithmetic",
    remote = "https://github.com/komori-n/saturation_arithmetic.git",
    # Replace the commit with the latest commit hash
    commit = "3069d8b5886e920184d60e9549bbe1c0a06ab60b",
)
```

### CMake

Add the following lines in `CMakeLists.txt`.

```cmake
include(FetchContent)
FetchContent_Declare(
    komori_saturation_arithmetic
    # Replace the commit with the latest commit hash
    URL "https://github.com/komori-n/saturation_arithmetic/archive/3069d8b5886e920184d60e9549bbe1c0a06ab60b.zip"
)
FetchContent_MakeAvailable(komori_saturation_arithmetic)
```

## Usage

```cpp
#include <cassert>
#include <cstdint>
#include <iostream>

#include <komori/saturation_arithmetic.hpp>

int main() {
    assert(komori::add_sat(std::int8_t{33}, std::int8_t{4}) == 37);
    assert(komori::add_sat(std::int8_t{100}, std::int8_t{100}) == 127);

    assert(komori::sub_sat(std::int8_t{33}, std::int8_t{4}) == 29);
    assert(komori::sub_sat(std::int8_t{-100}, std::int8_t{100}) == -128);

    assert(komori::mul_sat(std::int8_t{3}, std::int8_t{4}) == 12);
    assert(komori::mul_sat(std::int8_t{33}, std::int8_t{-4}) == -128);

    assert(komori::div_sat(std::int8_t{33}, std::int8_t{4}) == 8);
    assert(komori::div_sat(std::int8_t{-128}, std::int8_t{-1}) == 127);

    assert(komori::neg_sat(std::int8_t{33}) == -33);
    assert(komori::neg_sat(std::int8_t{-128}) == 127);

    assert(komori::saturate_cast<std::int8_t>(33) == 33);
    assert(komori::saturate_cast<std::int8_t>(334) == 127);

    komori::int_sat8_t a{33};
    assert(a + komori::int_sat8_t{100} == 127);
    assert(a * std::int8_t{-4} == -128);
}
```

## License

Apache License 2.0
