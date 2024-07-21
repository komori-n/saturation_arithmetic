#ifndef KOMORI_OPERATIONS_HPP_
#define KOMORI_OPERATIONS_HPP_

#include <limits>
#include <type_traits>

#if __cplusplus >= 201703L
#define KOMORI_CONSTEXPR_IF_CPP17 constexpr
#else
#define KOMORI_CONSTEXPR_IF_CPP17
#endif

#if defined(__has_builtin)
#define KOMORI_HAS_BUILTIN(x) __has_builtin(x)
#else
#define KOMORI_HAS_BUILTIN(x) 0
#endif

namespace komori {
namespace detail {
template <typename T>
constexpr T add_sat_wo_builtin(T x, T y) noexcept {
  constexpr T kMax = std::numeric_limits<T>::max();
  constexpr T kMin = std::numeric_limits<T>::min();

  if (y > 0 && x > kMax - y) {
    return kMax;
  } else if (y < 0 && x < kMin - y) {
    return kMin;
  } else {
    return x + y;
  }
}

template <typename T>
constexpr T sub_sat_wo_builtin(T x, T y) noexcept {
  constexpr T kMax = std::numeric_limits<T>::max();
  constexpr T kMin = std::numeric_limits<T>::min();

  if (y > 0 && x < kMin + y) {
    return kMin;
  } else if (y < 0 && x > kMax + y) {
    return kMax;
  } else {
    return x - y;
  }
}

template <typename T>
constexpr T mul_sat_wo_builtin(T x, T y) noexcept {
  constexpr T kMax = std::numeric_limits<T>::max();
  constexpr T kMin = std::numeric_limits<T>::min();

  if (x > 0 && y > 0) {
    if (x > kMax / y) {
      return kMax;
    }
  } else if (x < 0 && y < 0) {
    if (x < kMax / y) {
      return kMax;
    }
  } else if (x > 0 && y < 0) {
    if (y < kMin / x) {
      return kMin;
    }
  } else if (x < 0 && y > 0) {
    if (x < kMin / y) {
      return kMin;
    }
  }

  return x * y;
}
}  // namespace detail

/**
 * @brief Adds two integers with saturation.
 * @tparam T An integer type.
 * @param x The first operand.
 * @param y The second operand.
 * @return The sum of the two operands with saturation.
 */
template <typename T>
constexpr T add_sat(T x, T y) noexcept {
  // Use the built-in function if available.
#if KOMORI_HAS_BUILTIN(__builtin_add_overflow)
  T result{};
  if (__builtin_add_overflow(x, y, &result)) {
    return y >= 0 ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min();
  }
  return result;
#else
  return detail::add_sat_wo_builtin(x, y);
#endif
}

/**
 * @brief Subtracts two integers with saturation.
 * @tparam T An integer type.
 * @param x The minuend.
 * @param y The subtrahend.
 * @return The difference of the two operands with saturation.
 */
template <typename T>
constexpr T sub_sat(T x, T y) noexcept {
  // Use the built-in function if available.
#if KOMORI_HAS_BUILTIN(__builtin_sub_overflow)
  T result{};
  if (__builtin_sub_overflow(x, y, &result)) {
    return y >= 0 ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max();
  }
  return result;
#else
  return detail::sub_sat_wo_builtin(x, y);
#endif
}

/**
 * @brief Multiplies two integers with saturation.
 * @tparam T An integer type.
 * @param x The first operand.
 * @param y The second operand.
 * @return The product of the two operands with saturation.
 */
template <typename T>
constexpr T mul_sat(T x, T y) noexcept {
  // Use the built-in function if available.
#if KOMORI_HAS_BUILTIN(__builtin_mul_overflow)
  T result{};
  if (__builtin_mul_overflow(x, y, &result)) {
    return (x > 0) == (y > 0) ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min();
  }
  return result;
#else
  return detail::mul_sat_wo_builtin(x, y);
#endif
}

/**
 * @brief Divides two integers with saturation.
 * @tparam T An integer type.
 * @param x The dividend.
 * @param y The divisor.
 * @return The quotient of the division with saturation.
 * @pre `y` must not be zero.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, std::nullptr_t> = nullptr>
constexpr T div_sat(T x, T y) noexcept {
  if KOMORI_CONSTEXPR_IF_CPP17 (std::is_signed<T>::value) {
    if (x == std::numeric_limits<T>::min() && y == T{-1}) {
      return std::numeric_limits<T>::max();
    }
  }

  return x / y;
}

template <typename R, typename T>
constexpr R saturate_cast(T x) noexcept;
}  // namespace komori

#undef KOMORI_HAS_BUILTIN
#undef KOMORI_CONSTEXPR_IF_CPP17

#endif  // KOMORI_OPERATIONS_HPP_
