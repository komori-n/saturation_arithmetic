#include "komori/saturation_arithmetic.hpp"

#include <gtest/gtest.h>
#include <cstdint>
#include <limits>

using komori::add_sat;
using komori::div_sat;
using komori::int_sat64_t;
using komori::int_sat8_t;
using komori::mul_sat;
using komori::neg_sat;
using komori::saturate_cast;
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

template <typename T, typename S>
S type_clamp(S x) {
  return clamp<S>(x, std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
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

  EXPECT_EQ(static_cast<TypeParam>(33 + 4), add_sat(static_cast<TypeParam>(33), static_cast<TypeParam>(4)));
  EXPECT_EQ(static_cast<TypeParam>(33 + 4), add_sat_wo_builtin(static_cast<TypeParam>(33), static_cast<TypeParam>(4)));
  EXPECT_EQ(max, add_sat(max, static_cast<TypeParam>(1)));
  EXPECT_EQ(max, add_sat_wo_builtin(max, static_cast<TypeParam>(1)));

  if (std::is_signed<TypeParam>::value) {
    EXPECT_EQ(static_cast<TypeParam>(33 + -4), add_sat(static_cast<TypeParam>(33), static_cast<TypeParam>(-4)));
    EXPECT_EQ(static_cast<TypeParam>(33 + -4),
              add_sat_wo_builtin(static_cast<TypeParam>(33), static_cast<TypeParam>(-4)));
    EXPECT_EQ(min, add_sat(min, static_cast<TypeParam>(-1)));
    EXPECT_EQ(min, add_sat_wo_builtin(min, static_cast<TypeParam>(-1)));
    EXPECT_EQ(min + 1, add_sat(min, static_cast<TypeParam>(1)));
    EXPECT_EQ(min + 1, add_sat_wo_builtin(min, static_cast<TypeParam>(1)));
    EXPECT_EQ(max, add_sat(max, static_cast<TypeParam>(1)));
    EXPECT_EQ(max, add_sat_wo_builtin(max, static_cast<TypeParam>(1)));
    EXPECT_EQ(max - 1, add_sat(max, static_cast<TypeParam>(-1)));
    EXPECT_EQ(max - 1, add_sat_wo_builtin(max, static_cast<TypeParam>(-1)));
  }
}

TYPED_TEST_SUITE(SubSatTest, integers);
TYPED_TEST(SubSatTest, Test) {
  constexpr TypeParam min = std::numeric_limits<TypeParam>::min();
  constexpr TypeParam max = std::numeric_limits<TypeParam>::max();

  EXPECT_EQ(static_cast<TypeParam>(33 - 4), sub_sat(static_cast<TypeParam>(33), static_cast<TypeParam>(4)));
  EXPECT_EQ(static_cast<TypeParam>(33 - 4), sub_sat_wo_builtin(static_cast<TypeParam>(33), static_cast<TypeParam>(4)));
  EXPECT_EQ(min, sub_sat(min, static_cast<TypeParam>(1)));
  EXPECT_EQ(min, sub_sat_wo_builtin(min, static_cast<TypeParam>(1)));

  if (std::is_signed<TypeParam>::value) {
    EXPECT_EQ(static_cast<TypeParam>(33 - -4), sub_sat(static_cast<TypeParam>(33), static_cast<TypeParam>(-4)));
    EXPECT_EQ(static_cast<TypeParam>(33 - -4),
              sub_sat_wo_builtin(static_cast<TypeParam>(33), static_cast<TypeParam>(-4)));
    EXPECT_EQ(min, sub_sat(min, static_cast<TypeParam>(1)));
    EXPECT_EQ(min, sub_sat_wo_builtin(min, static_cast<TypeParam>(1)));
    EXPECT_EQ(min + 1, sub_sat(min, static_cast<TypeParam>(-1)));
    EXPECT_EQ(min + 1, sub_sat_wo_builtin(min, static_cast<TypeParam>(-1)));
    EXPECT_EQ(max, sub_sat(max, static_cast<TypeParam>(-1)));
    EXPECT_EQ(max, sub_sat_wo_builtin(max, static_cast<TypeParam>(-1)));
    EXPECT_EQ(max - 1, sub_sat(max, static_cast<TypeParam>(1)));
    EXPECT_EQ(max - 1, sub_sat_wo_builtin(max, static_cast<TypeParam>(1)));
  }
}

TYPED_TEST_SUITE(MulSatTest, integers);
TYPED_TEST(MulSatTest, Test) {
  constexpr TypeParam min = std::numeric_limits<TypeParam>::min();
  constexpr TypeParam max = std::numeric_limits<TypeParam>::max();

  EXPECT_EQ(static_cast<TypeParam>(3 * 4), mul_sat(static_cast<TypeParam>(3), static_cast<TypeParam>(4)));
  EXPECT_EQ(static_cast<TypeParam>(3 * 4), mul_sat_wo_builtin(static_cast<TypeParam>(3), static_cast<TypeParam>(4)));
  EXPECT_EQ(max, mul_sat(static_cast<TypeParam>(max - 33), static_cast<TypeParam>(4)));
  EXPECT_EQ(max, mul_sat_wo_builtin(static_cast<TypeParam>(max - 33), static_cast<TypeParam>(4)));

  if (std::is_signed<TypeParam>::value) {
    EXPECT_EQ(min, mul_sat(static_cast<TypeParam>(min + 33), static_cast<TypeParam>(4)));
    EXPECT_EQ(min, mul_sat_wo_builtin(static_cast<TypeParam>(min + 33), static_cast<TypeParam>(4)));
    EXPECT_EQ(max, mul_sat(static_cast<TypeParam>(min + 33), static_cast<TypeParam>(-4)));
    EXPECT_EQ(max, mul_sat_wo_builtin(static_cast<TypeParam>(min + 33), static_cast<TypeParam>(-4)));
    EXPECT_EQ(min, mul_sat(static_cast<TypeParam>(max - 33), static_cast<TypeParam>(-4)));
    EXPECT_EQ(min, mul_sat_wo_builtin(static_cast<TypeParam>(max - 33), static_cast<TypeParam>(-4)));
    EXPECT_EQ(max, mul_sat(static_cast<TypeParam>(max - 33), static_cast<TypeParam>(4)));
    EXPECT_EQ(max, mul_sat_wo_builtin(static_cast<TypeParam>(max - 33), static_cast<TypeParam>(4)));
  }
}

TYPED_TEST_SUITE(DivSatTest, integers);
TYPED_TEST(DivSatTest, Test) {
  constexpr TypeParam min = std::numeric_limits<TypeParam>::min();
  constexpr TypeParam max = std::numeric_limits<TypeParam>::max();

  EXPECT_EQ(static_cast<TypeParam>(33 / 4), div_sat(static_cast<TypeParam>(33), static_cast<TypeParam>(4)));
  EXPECT_EQ(min, div_sat(min, static_cast<TypeParam>(1)));
  EXPECT_EQ(max, div_sat(max, static_cast<TypeParam>(1)));

  if (std::is_signed<TypeParam>::value) {
    EXPECT_EQ(static_cast<TypeParam>(33 / -4), div_sat(static_cast<TypeParam>(33), static_cast<TypeParam>(-4)));
    EXPECT_EQ(max, div_sat(min, static_cast<TypeParam>(-1)));
    EXPECT_EQ(static_cast<TypeParam>(-max), div_sat(max, static_cast<TypeParam>(-1)));
  }
}

TEST(S8Test, Int8All) {
  const std::int32_t s8min = std::numeric_limits<std::int8_t>::min();
  const std::int32_t s8max = std::numeric_limits<std::int8_t>::max();

  for (std::int32_t x = s8min; x <= s8max; ++x) {
    const std::int32_t expected_neg = clamp(-x, s8min, s8max);
    const std::int32_t actual_neg1 = neg_sat(static_cast<std::int8_t>(x));
    EXPECT_EQ(expected_neg, actual_neg1) << "x: " << x;

    for (std::int32_t y = s8min; y <= s8max; ++y) {
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

      if (y == 0) {
        continue;
      }

      const std::int32_t expected_div = clamp(x / y, s8min, s8max);
      const std::int32_t actual_div = div_sat(static_cast<std::int8_t>(x), static_cast<std::int8_t>(y));
      ASSERT_EQ(expected_div, actual_div) << "x: " << x << ", y: " << y;
    }
  }
}

TEST(SaturateCast, Uint16All) {
  const std::int64_t u16max = std::numeric_limits<std::uint16_t>::max();

  for (std::int64_t x = 0; x <= u16max; ++x) {
    std::uint16_t value = static_cast<std::uint16_t>(x);
    ASSERT_EQ(type_clamp<std::uint32_t>(x), saturate_cast<std::uint32_t>(value)) << "x: " << x;
    ASSERT_EQ(type_clamp<std::int32_t>(x), saturate_cast<std::int32_t>(value)) << "x: " << x;
    ASSERT_EQ(type_clamp<std::uint16_t>(x), saturate_cast<std::uint16_t>(value)) << "x: " << x;
    ASSERT_EQ(type_clamp<std::int16_t>(x), saturate_cast<std::int16_t>(value)) << "x: " << x;
    ASSERT_EQ(type_clamp<std::uint8_t>(x), saturate_cast<std::uint8_t>(value)) << "x: " << x;
    ASSERT_EQ(type_clamp<std::int8_t>(x), saturate_cast<std::int8_t>(value)) << "x: " << x;
  }
}

TEST(SaturateCast, Int16All) {
  const std::int64_t s16min = std::numeric_limits<std::int16_t>::min();
  const std::int64_t s16max = std::numeric_limits<std::int16_t>::max();
  for (std::int64_t x = s16min; x <= s16max; ++x) {
    std::int16_t value = static_cast<std::int16_t>(x);
    ASSERT_EQ(type_clamp<std::uint32_t>(x), saturate_cast<std::uint32_t>(value)) << "x: " << x;
    ASSERT_EQ(type_clamp<std::int32_t>(x), saturate_cast<std::int32_t>(value)) << "x: " << x;
    ASSERT_EQ(type_clamp<std::uint16_t>(x), saturate_cast<std::uint16_t>(value)) << "x: " << x;
    ASSERT_EQ(type_clamp<std::int16_t>(x), saturate_cast<std::int16_t>(value)) << "x: " << x;
    ASSERT_EQ(type_clamp<std::uint8_t>(x), saturate_cast<std::uint8_t>(value)) << "x: " << x;
    ASSERT_EQ(type_clamp<std::int8_t>(x), saturate_cast<std::int8_t>(value)) << "x: " << x;
  }
}

TEST(SatTypeTest, TypeConversion) {
  const std::int8_t s8min = std::numeric_limits<std::int8_t>::min();
  const std::int8_t s8max = std::numeric_limits<std::int8_t>::max();

  for (std::int32_t x = s8min - 1; x <= s8max + 1; ++x) {
    const int_sat8_t x_sat8{x};  // explicit constructor
    const std::int8_t x_sat8_expected = saturate_cast<std::int8_t>(x);

    EXPECT_EQ(x_sat8, x_sat8_expected) << "x: " << x;
    EXPECT_EQ(x_sat8.value(), x_sat8_expected) << "x: " << x;

    const int_sat64_t x_sat64 = x;  // non-explicit constructor
    EXPECT_EQ(x_sat64, x) << "x: " << x;

    int_sat64_t y_sat64;
    y_sat64 = x;  // assignment operator
    EXPECT_EQ(y_sat64, x) << "x: " << x;

    EXPECT_EQ(static_cast<int_sat8_t>(x_sat64), x_sat8) << "x: " << x;
    EXPECT_EQ(static_cast<std::int8_t>(x_sat64), x_sat8_expected) << "x: " << x;

    EXPECT_EQ(static_cast<bool>(x_sat8), x_sat8 != 0) << "x: " << x;
  }
}

TEST(SatTypeTest, Comparisons) {
  const std::int8_t s8min = std::numeric_limits<std::int8_t>::min();
  const std::int8_t s8max = std::numeric_limits<std::int8_t>::max();

  for (std::int64_t x = s8min; x <= s8max; ++x) {
    for (std::int64_t y = s8min; y <= s8max; ++y) {
      const std::int8_t x_s8 = static_cast<std::int8_t>(x);
      const std::int8_t y_s8 = static_cast<std::int8_t>(y);
      const int_sat8_t x_sat8 = x_s8;
      const int_sat8_t y_sat8 = y_s8;

      const bool expected_lt = x_s8 < y_s8;
      ASSERT_EQ(x_sat8 < y_sat8, expected_lt) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_sat8 < y_s8, expected_lt) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_s8 < y_sat8, expected_lt) << "x: " << x << ", y: " << y;

      const bool expected_le = x_s8 <= y_s8;
      ASSERT_EQ(x_sat8 <= y_sat8, expected_le) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_sat8 <= y_s8, expected_le) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_s8 <= y_sat8, expected_le) << "x: " << x << ", y: " << y;

      const bool expected_gt = x_s8 > y_s8;
      ASSERT_EQ(x_sat8 > y_sat8, expected_gt) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_sat8 > y_s8, expected_gt) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_s8 > y_sat8, expected_gt) << "x: " << x << ", y: " << y;

      const bool expected_ge = x_s8 >= y_s8;
      ASSERT_EQ(x_sat8 >= y_sat8, expected_ge) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_sat8 >= y_s8, expected_ge) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_s8 >= y_sat8, expected_ge) << "x: " << x << ", y: " << y;

      const bool expected_eq = x_s8 == y_s8;
      ASSERT_EQ(x_sat8 == y_sat8, expected_eq) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_sat8 == y_s8, expected_eq) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_s8 == y_sat8, expected_eq) << "x: " << x << ", y: " << y;

      const bool expected_ne = x_s8 != y_s8;
      ASSERT_EQ(x_sat8 != y_sat8, expected_ne) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_sat8 != y_s8, expected_ne) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_s8 != y_sat8, expected_ne) << "x: " << x << ", y: " << y;
    }
  }
}

TEST(SatTypeTest, Arithmetics) {
  const std::int8_t s8min = std::numeric_limits<std::int8_t>::min();
  const std::int8_t s8max = std::numeric_limits<std::int8_t>::max();

  for (std::int64_t x = s8min; x <= s8max; ++x) {
    for (std::int64_t y = s8min; y <= s8max; ++y) {
      const std::int8_t x_s8 = static_cast<std::int8_t>(x);
      const std::int8_t y_s8 = static_cast<std::int8_t>(y);
      const int_sat8_t x_sat8 = x_s8;
      const int_sat8_t y_sat8 = y_s8;
      int_sat8_t tmp;

      const int_sat8_t expected_neg = static_cast<std::int8_t>(clamp<std::int64_t>(-x, s8min, s8max));
      ASSERT_EQ(-x_sat8, expected_neg) << "x: " << x;

      const int_sat8_t expected_add = static_cast<std::int8_t>(clamp<std::int64_t>(x + y, s8min, s8max));
      ASSERT_EQ(x_sat8 + y_sat8, expected_add) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_sat8 + y_s8, expected_add) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_s8 + y_sat8, expected_add) << "x: " << x << ", y: " << y;
      tmp = x_sat8;
      ASSERT_EQ(tmp += y_sat8, expected_add) << "x: " << x << ", y: " << y;
      ASSERT_EQ(tmp, expected_add) << "x: " << x << ", y: " << y;
      tmp = x_sat8;
      ASSERT_EQ(tmp += y_s8, expected_add) << "x: " << x << ", y: " << y;
      ASSERT_EQ(tmp, expected_add) << "x: " << x << ", y: " << y;

      const int_sat8_t expected_sub = static_cast<std::int8_t>(clamp<std::int64_t>(x - y, s8min, s8max));
      ASSERT_EQ(x_sat8 - y_sat8, expected_sub) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_sat8 - y_s8, expected_sub) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_s8 - y_sat8, expected_sub) << "x: " << x << ", y: " << y;
      tmp = x_sat8;
      ASSERT_EQ(tmp -= y_sat8, expected_sub) << "x: " << x << ", y: " << y;
      ASSERT_EQ(tmp, expected_sub) << "x: " << x << ", y: " << y;
      tmp = x_sat8;
      ASSERT_EQ(tmp -= y_s8, expected_sub) << "x: " << x << ", y: " << y;
      ASSERT_EQ(tmp, expected_sub) << "x: " << x << ", y: " << y;

      const int_sat8_t expected_mul = static_cast<std::int8_t>(clamp<std::int64_t>(x * y, s8min, s8max));
      ASSERT_EQ(x_sat8 * y_sat8, expected_mul) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_sat8 * y_s8, expected_mul) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_s8 * y_sat8, expected_mul) << "x: " << x << ", y: " << y;
      tmp = x_sat8;
      ASSERT_EQ(tmp *= y_sat8, expected_mul) << "x: " << x << ", y: " << y;
      ASSERT_EQ(tmp, expected_mul) << "x: " << x << ", y: " << y;
      tmp = x_sat8;
      ASSERT_EQ(tmp *= y_s8, expected_mul) << "x: " << x << ", y: " << y;
      ASSERT_EQ(tmp, expected_mul) << "x: " << x << ", y: " << y;

      if (y == 0) {
        continue;
      }
      const int_sat8_t expected_div = static_cast<std::int8_t>(clamp<std::int64_t>(x / y, s8min, s8max));
      ASSERT_EQ(x_sat8 / y_sat8, expected_div) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_sat8 / y_s8, expected_div) << "x: " << x << ", y: " << y;
      ASSERT_EQ(x_s8 / y_sat8, expected_div) << "x: " << x << ", y: " << y;
      tmp = x_sat8;
      ASSERT_EQ(tmp /= y_sat8, expected_div) << "x: " << x << ", y: " << y;
      ASSERT_EQ(tmp, expected_div) << "x: " << x << ", y: " << y;
      tmp = x_sat8;
      ASSERT_EQ(tmp /= y_s8, expected_div) << "x: " << x << ", y: " << y;
      ASSERT_EQ(tmp, expected_div) << "x: " << x << ", y: " << y;
    }
  }
}

TEST(SatTypeTest, OtherOperators) {
  const std::int8_t s8min = std::numeric_limits<std::int8_t>::min();
  const std::int8_t s8max = std::numeric_limits<std::int8_t>::max();

  for (std::int64_t x = s8min; x <= s8max; ++x) {
    const std::int8_t x_s8 = static_cast<std::int8_t>(x);
    const int_sat8_t x_sat8 = x_s8;
    int_sat8_t tmp;

    const int_sat8_t expected_neg = static_cast<std::int8_t>(clamp<std::int64_t>(-x, s8min, s8max));
    ASSERT_EQ(-x_sat8, expected_neg) << "x: " << x;

    tmp = x_sat8;
    ASSERT_EQ(tmp++, x_sat8) << "x: " << x;
    ASSERT_EQ(tmp, x_sat8 + std::int8_t{1}) << "x: " << x;

    tmp = x_sat8;
    ASSERT_EQ(++tmp, x_sat8 + std::int8_t{1}) << "x: " << x;
    ASSERT_EQ(tmp, x_sat8 + std::int8_t{1}) << "x: " << x;

    tmp = x_sat8;
    ASSERT_EQ(tmp--, x_sat8) << "x: " << x;
    ASSERT_EQ(tmp, x_sat8 - std::int8_t{1}) << "x: " << x;

    tmp = x_sat8;
    ASSERT_EQ(--tmp, x_sat8 - std::int8_t{1}) << "x: " << x;
    ASSERT_EQ(tmp, x_sat8 - std::int8_t{1}) << "x: " << x;
  }
}
