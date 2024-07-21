#include "komori/operations.hpp"

#include <gtest/gtest.h>
#include <cstdint>
#include <limits>

using komori::add_sat;
using komori::div_sat;
using komori::mul_sat;
using komori::sub_sat;
using komori::detail::add_sat_wo_builtin;
using komori::detail::mul_sat_wo_builtin;
using komori::detail::sub_sat_wo_builtin;

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
class AddSatTest : public testing::Test {};
template <typename T>
class SubSatTest : public testing::Test {};
template <typename T>
class MulSatTest : public testing::Test {};
template <typename T>
class DivSatTest : public testing::Test {};
}  // namespace

TYPED_TEST_SUITE(AddSatTest, integers);
TYPED_TEST(AddSatTest, Test) {
  constexpr TypeParam min = std::numeric_limits<TypeParam>::min();
  constexpr TypeParam max = std::numeric_limits<TypeParam>::max();

  EXPECT_EQ(TypeParam{33 + 4}, add_sat(TypeParam{33}, TypeParam{4}));
  EXPECT_EQ(TypeParam{33 + 4}, add_sat_wo_builtin(TypeParam{33}, TypeParam{4}));
  EXPECT_EQ(max, add_sat(max, TypeParam{1}));
  EXPECT_EQ(max, add_sat_wo_builtin(max, TypeParam{1}));

  if (std::is_signed<TypeParam>::value) {
    EXPECT_EQ(TypeParam{33 + -4}, add_sat(TypeParam{33}, TypeParam{-4}));
    EXPECT_EQ(TypeParam{33 + -4}, add_sat_wo_builtin(TypeParam{33}, TypeParam{-4}));
    EXPECT_EQ(min, add_sat(min, TypeParam{-1}));
    EXPECT_EQ(min, add_sat_wo_builtin(min, TypeParam{-1}));
    EXPECT_EQ(min + 1, add_sat(min, TypeParam{1}));
    EXPECT_EQ(min + 1, add_sat_wo_builtin(min, TypeParam{1}));
    EXPECT_EQ(max, add_sat(max, TypeParam{1}));
    EXPECT_EQ(max, add_sat_wo_builtin(max, TypeParam{1}));
    EXPECT_EQ(max - 1, add_sat(max, TypeParam{-1}));
    EXPECT_EQ(max - 1, add_sat_wo_builtin(max, TypeParam{-1}));
  }
}

TYPED_TEST_SUITE(SubSatTest, integers);
TYPED_TEST(SubSatTest, Test) {
  constexpr TypeParam min = std::numeric_limits<TypeParam>::min();
  constexpr TypeParam max = std::numeric_limits<TypeParam>::max();

  EXPECT_EQ(TypeParam{33 - 4}, sub_sat(TypeParam{33}, TypeParam{4}));
  EXPECT_EQ(TypeParam{33 - 4}, sub_sat_wo_builtin(TypeParam{33}, TypeParam{4}));
  EXPECT_EQ(min, sub_sat(min, TypeParam{1}));
  EXPECT_EQ(min, sub_sat_wo_builtin(min, TypeParam{1}));

  if (std::is_signed<TypeParam>::value) {
    EXPECT_EQ(TypeParam{33 - -4}, sub_sat(TypeParam{33}, TypeParam{-4}));
    EXPECT_EQ(TypeParam{33 - -4}, sub_sat_wo_builtin(TypeParam{33}, TypeParam{-4}));
    EXPECT_EQ(min, sub_sat(min, TypeParam{1}));
    EXPECT_EQ(min, sub_sat_wo_builtin(min, TypeParam{1}));
    EXPECT_EQ(min + 1, sub_sat(min, TypeParam{-1}));
    EXPECT_EQ(min + 1, sub_sat_wo_builtin(min, TypeParam{-1}));
    EXPECT_EQ(max, sub_sat(max, TypeParam{-1}));
    EXPECT_EQ(max, sub_sat_wo_builtin(max, TypeParam{-1}));
    EXPECT_EQ(max - 1, sub_sat(max, TypeParam{1}));
    EXPECT_EQ(max - 1, sub_sat_wo_builtin(max, TypeParam{1}));
  }
}

TYPED_TEST_SUITE(MulSatTest, integers);
TYPED_TEST(MulSatTest, Test) {
  constexpr TypeParam min = std::numeric_limits<TypeParam>::min();
  constexpr TypeParam max = std::numeric_limits<TypeParam>::max();

  EXPECT_EQ(TypeParam{3 * 4}, mul_sat(TypeParam{3}, TypeParam{4}));
  EXPECT_EQ(TypeParam{3 * 4}, mul_sat_wo_builtin(TypeParam{3}, TypeParam{4}));
  EXPECT_EQ(max, mul_sat(TypeParam{max - 33}, TypeParam{4}));
  EXPECT_EQ(max, mul_sat_wo_builtin(TypeParam{max - 33}, TypeParam{4}));

  if (std::is_signed<TypeParam>::value) {
    EXPECT_EQ(min, mul_sat(TypeParam{min + 33}, TypeParam{4}));
    EXPECT_EQ(min, mul_sat_wo_builtin(TypeParam{min + 33}, TypeParam{4}));
    EXPECT_EQ(max, mul_sat(TypeParam{min + 33}, TypeParam{-4}));
    EXPECT_EQ(max, mul_sat_wo_builtin(TypeParam{min + 33}, TypeParam{-4}));
    EXPECT_EQ(min, mul_sat(TypeParam{max - 33}, TypeParam{-4}));
    EXPECT_EQ(min, mul_sat_wo_builtin(TypeParam{max - 33}, TypeParam{-4}));
    EXPECT_EQ(max, mul_sat(TypeParam{max - 33}, TypeParam{4}));
    EXPECT_EQ(max, mul_sat_wo_builtin(TypeParam{max - 33}, TypeParam{4}));
  }
}

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

      const std::int32_t expected_add = clamp(x + y, s8min, s8max);
      const std::int32_t actual_add1 = add_sat(static_cast<std::int8_t>(x), static_cast<std::int8_t>(y));
      const std::int32_t actual_add2 = add_sat_wo_builtin(static_cast<std::int8_t>(x), static_cast<std::int8_t>(y));
      ASSERT_EQ(expected_add, actual_add1) << "x: " << x << ", y: " << y;
      ASSERT_EQ(expected_add, actual_add2) << "x: " << x << ", y: " << y;

      const std::int32_t expected_sub = clamp(x - y, s8min, s8max);
      const std::int32_t actual_sub1 = sub_sat(static_cast<std::int8_t>(x), static_cast<std::int8_t>(y));
      const std::int32_t actual_sub2 = sub_sat_wo_builtin(static_cast<std::int8_t>(x), static_cast<std::int8_t>(y));
      ASSERT_EQ(expected_sub, actual_sub1) << "x: " << x << ", y: " << y;
      ASSERT_EQ(expected_sub, actual_sub2) << "x: " << x << ", y: " << y;

      const std::int32_t expected_mul = clamp(x * y, s8min, s8max);
      const std::int32_t actual_mul1 = mul_sat(static_cast<std::int8_t>(x), static_cast<std::int8_t>(y));
      const std::int32_t actual_mul2 = mul_sat_wo_builtin(static_cast<std::int8_t>(x), static_cast<std::int8_t>(y));
      ASSERT_EQ(expected_mul, actual_mul1) << "x: " << x << ", y: " << y;
      ASSERT_EQ(expected_mul, actual_mul2) << "x: " << x << ", y: " << y;

      const std::int32_t expected_div = clamp(x / y, s8min, s8max);
      const std::int32_t actual_div = div_sat(static_cast<std::int8_t>(x), static_cast<std::int8_t>(y));
      ASSERT_EQ(expected_div, actual_div) << "x: " << x << ", y: " << y;
    }
  }
}
