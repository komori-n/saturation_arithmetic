#include "komori/operations.hpp"

#include <gtest/gtest.h>
#include <cstdint>
#include <limits>

using komori::div_sat;

namespace {
template <typename T>
T clamp(T x, T min, T max) {
  if (x < min) {
    return min;
  }
  if (x > max) {
    return max;
  }
  return x;
}

using integers = testing::Types<std::int8_t,
                                std::int16_t,
                                std::int32_t,
                                std::int64_t,
                                std::uint8_t,
                                std::uint16_t,
                                std::uint32_t,
                                std::uint64_t>;

template <typename T>
class DivSatTest : public testing::Test {};
}  // namespace

TYPED_TEST_SUITE(DivSatTest, integers);
TYPED_TEST(DivSatTest, Test) {
  constexpr TypeParam min = std::numeric_limits<TypeParam>::min();
  constexpr TypeParam max = std::numeric_limits<TypeParam>::max();

  EXPECT_EQ(TypeParam{33 / 4}, div_sat(TypeParam{33}, TypeParam{4}));
  EXPECT_EQ(min, div_sat(min, TypeParam{1}));
  EXPECT_EQ(max, div_sat(max, TypeParam{1}));

  if (std::is_signed<TypeParam>::value) {
    EXPECT_EQ(TypeParam{33 / -4}, div_sat(TypeParam{33}, TypeParam{-4}));
    EXPECT_EQ(max, div_sat(min, TypeParam{-1}));
    EXPECT_EQ(TypeParam{-max}, div_sat(max, TypeParam{-1}));
  }
}

TEST(S8Test, Int8All) {
  const std::int32_t s8min = std::numeric_limits<std::int8_t>::min();
  const std::int32_t s8max = std::numeric_limits<std::int8_t>::max();

  for (std::int32_t x = s8min; x <= s8max; ++x) {
    for (std::int32_t y = s8min; y <= s8max; ++y) {
      if (y == 0) {
        continue;
      }

      const std::int32_t expected = clamp(x / y, s8min, s8max);
      const std::int32_t actual = div_sat(static_cast<std::int8_t>(x), static_cast<std::int8_t>(y));

      ASSERT_EQ(expected, actual) << "x: " << x << ", y: " << y;
    }
  }
}
