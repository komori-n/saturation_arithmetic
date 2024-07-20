#ifndef KOMORI_OPERATIONS_HPP_
#define KOMORI_OPERATIONS_HPP_

#include <limits>
#include <type_traits>

#if __cplusplus >= 201703L
#define KOMORI_CONSTEXPR_IF_CPP17 constexpr
#else
#define KOMORI_CONSTEXPR_IF_CPP17
#endif

namespace komori {
template <typename T>
constexpr T add_sat(T x, T y) noexcept;
template <typename T>
constexpr T sub_sat(T x, T y) noexcept;
template <typename T>
constexpr T mul_sat(T x, T y) noexcept;

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

#undef KOMORI_CONSTEXPR_IF_CPP17

#endif  // KOMORI_OPERATIONS_HPP_
