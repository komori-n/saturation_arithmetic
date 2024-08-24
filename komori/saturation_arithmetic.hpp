#ifndef KOMORI_OPERATIONS_HPP_
#define KOMORI_OPERATIONS_HPP_

#include <cstdint>
#include <functional>
#include <limits>
#include <type_traits>

#if __cplusplus >= 201703L
#define KOMORI_CONSTEXPR_CPP17 constexpr
#else
#define KOMORI_CONSTEXPR_CPP17
#endif

#if __cplusplus >= 202002L
#define KOMORI_CONSTEXPR_CPP20 constexpr
#else
#define KOMORI_CONSTEXPR_CPP20
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
template <typename T, std::enable_if_t<std::is_integral<T>::value, std::nullptr_t> = nullptr>
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
template <typename T, std::enable_if_t<std::is_integral<T>::value, std::nullptr_t> = nullptr>
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
template <typename T, std::enable_if_t<std::is_integral<T>::value, std::nullptr_t> = nullptr>
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
 * @brief Negates an integer with saturation.
 * @tparam T An integer type.
 * @param x The value to negate.
 * @return The negated value with saturation.
 */
template <typename T, std::enable_if_t<std::is_integral<T>::value, std::nullptr_t> = nullptr>
constexpr T neg_sat(T x) noexcept {
  return x == std::numeric_limits<T>::min() ? std::numeric_limits<T>::max() : -x;
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
  if KOMORI_CONSTEXPR_CPP17 (std::is_signed<T>::value) {
    if (y == static_cast<T>(-1)) {
      return neg_sat(x);
    }
  }

  return x / y;
}

/**
 * @brief Casts a value to another type with saturation.
 * @tparam R The destination type. (integral type)
 * @tparam T An integer type.
 * @param x The value to cast.
 * @return The value casted to the destination type with saturation.
 */
template <typename R,
          typename T,
          std::enable_if_t<std::is_integral<R>::value && std::is_integral<T>::value, std::nullptr_t> = nullptr>
constexpr R saturate_cast(T x) noexcept {
  using ST = std::make_signed_t<T>;
  using SR = std::make_signed_t<R>;
  using UT = std::make_unsigned_t<T>;
  using UR = std::make_unsigned_t<R>;

  if (x < 0) {
    if KOMORI_CONSTEXPR_CPP17 (std::is_unsigned<R>::value) {
      return 0;
    } else if (static_cast<SR>(std::numeric_limits<R>::min()) > static_cast<ST>(x)) {
      return std::numeric_limits<R>::min();
    }
  } else if (static_cast<UR>(std::numeric_limits<R>::max()) < static_cast<UT>(x)) {
    return std::numeric_limits<R>::max();
  }

  return static_cast<R>(x);
}

namespace detail {
template <typename T,
          typename U,
          std::enable_if_t<std::is_integral<T>::value && std::is_integral<U>::value, std::nullptr_t> = nullptr>
struct is_same_signedness : std::integral_constant<bool, std::is_signed<T>::value == std::is_signed<U>::value> {};

template <typename T,
          typename U,
          std::enable_if_t<std::is_integral<T>::value && std::is_integral<U>::value, std::nullptr_t> = nullptr>
struct promoted_type {
  using type = std::
      conditional_t<is_same_signedness<T, U>::value, std::conditional_t<(sizeof(T) > sizeof(U)), T, U>, std::nullptr_t>;
};

template <typename T>
class sat_t {
  static_assert(std::is_integral<T>::value, "T must be an integral type.");

 public:
  KOMORI_CONSTEXPR_CPP20 sat_t() noexcept = default;
  template <typename U,
            std::enable_if_t<std::is_same<T, typename promoted_type<T, U>::type>::value, std::nullptr_t> = nullptr>
  constexpr sat_t(U x) noexcept : value_(static_cast<T>(x)) {}

  template <typename U,
            std::enable_if_t<!std::is_same<T, typename promoted_type<T, U>::type>::value, std::nullptr_t> = nullptr>
  explicit constexpr sat_t(U x) noexcept : value_(saturate_cast<T>(x)) {}

  constexpr sat_t(const sat_t&) noexcept = default;
  constexpr sat_t(sat_t&&) noexcept = default;
  template <typename U,
            std::enable_if_t<std::is_same<T, typename promoted_type<T, U>::type>::value, std::nullptr_t> = nullptr>
  constexpr sat_t& operator=(U x) noexcept {
    value_ = static_cast<T>(x);
    return *this;
  }
  constexpr sat_t& operator=(const sat_t&) noexcept = default;
  constexpr sat_t& operator=(sat_t&&) noexcept = default;
  KOMORI_CONSTEXPR_CPP20 ~sat_t() noexcept = default;

  template <typename U>
  constexpr operator sat_t<U>() const noexcept {
    return {saturate_cast<U>(value_)};
  }

  template <typename U,
            std::enable_if_t<std::is_same<U, typename promoted_type<T, U>::type>::value, std::nullptr_t> = nullptr>
  constexpr operator U() const noexcept {
    return value_;
  }

  template <typename U,
            std::enable_if_t<!std::is_same<U, typename promoted_type<T, U>::type>::value, std::nullptr_t> = nullptr>
  explicit constexpr operator U() const noexcept {
    return saturate_cast<U>(value_);
  }

  explicit constexpr operator bool() const noexcept { return value_ != 0; }
  constexpr T value() const noexcept { return static_cast<T>(value_); }

 private:
  T value_;
};

#define KOMORI_DEFINE_SATURATED_COMPARISON_OPERATORS(op)                                                         \
  template <typename T, typename U, std::enable_if_t<is_same_signedness<T, U>::value, std::nullptr_t> = nullptr> \
  constexpr bool operator op(sat_t<T> x, sat_t<U> y) noexcept {                                                  \
    return x.value() op y.value();                                                                               \
  }                                                                                                              \
  template <typename T, typename U, std::enable_if_t<is_same_signedness<T, U>::value, std::nullptr_t> = nullptr> \
  constexpr bool operator op(sat_t<T> x, U y) noexcept {                                                         \
    return x.value() op y;                                                                                       \
  }                                                                                                              \
  template <typename T, typename U, std::enable_if_t<is_same_signedness<T, U>::value, std::nullptr_t> = nullptr> \
  constexpr bool operator op(T x, sat_t<U> y) noexcept {                                                         \
    return x op y.value();                                                                                       \
  }

KOMORI_DEFINE_SATURATED_COMPARISON_OPERATORS(==);
KOMORI_DEFINE_SATURATED_COMPARISON_OPERATORS(!=);
KOMORI_DEFINE_SATURATED_COMPARISON_OPERATORS(<);
KOMORI_DEFINE_SATURATED_COMPARISON_OPERATORS(>);
KOMORI_DEFINE_SATURATED_COMPARISON_OPERATORS(<=);
KOMORI_DEFINE_SATURATED_COMPARISON_OPERATORS(>=);

#undef KOMORI_DEFINE_SATURATED_COMPARISON_OPERATORS

#define KOMORI_DEFINE_SATURATED_ARITHMETIC_OPERATORS(op, op_sat)                                                    \
  template <typename T, typename U, std::enable_if_t<is_same_signedness<T, U>::value, std::nullptr_t> = nullptr>    \
  constexpr sat_t<typename promoted_type<T, U>::type> operator op(sat_t<T> x, sat_t<U> y) noexcept {                \
    using promoted = typename promoted_type<T, U>::type;                                                            \
    return op_sat(static_cast<promoted>(x.value()), static_cast<promoted>(y.value()));                              \
  }                                                                                                                 \
  template <typename T, typename U,                                                                                 \
            std::enable_if_t<std::is_same<T, typename promoted_type<T, U>::type>::value, std::nullptr_t> = nullptr> \
  constexpr sat_t<T> operator op(sat_t<T> x, U y) noexcept {                                                        \
    return op_sat(x.value(), static_cast<T>(y));                                                                    \
  }                                                                                                                 \
  template <typename T, typename U,                                                                                 \
            std::enable_if_t<std::is_same<U, typename promoted_type<T, U>::type>::value, std::nullptr_t> = nullptr> \
  constexpr sat_t<U> operator op(T x, sat_t<U> y) noexcept {                                                        \
    return op_sat(static_cast<U>(x), y.value());                                                                    \
  }                                                                                                                 \
  template <typename T, typename U,                                                                                 \
            std::enable_if_t<std::is_same<T, typename promoted_type<T, U>::type>::value, std::nullptr_t> = nullptr> \
  constexpr sat_t<T>& operator op##=(sat_t<T>& x, sat_t<U> y) noexcept {                                            \
    x = op_sat(x.value(), static_cast<T>(y.value()));                                                               \
    return x;                                                                                                       \
  }                                                                                                                 \
  template <typename T, typename U,                                                                                 \
            std::enable_if_t<std::is_same<T, typename promoted_type<T, U>::type>::value, std::nullptr_t> = nullptr> \
  constexpr sat_t<T>& operator op##=(sat_t<T>& x, U y) noexcept {                                                   \
    x = op_sat(x.value(), static_cast<T>(y));                                                                       \
    return x;                                                                                                       \
  }

KOMORI_DEFINE_SATURATED_ARITHMETIC_OPERATORS(+, add_sat);
KOMORI_DEFINE_SATURATED_ARITHMETIC_OPERATORS(-, sub_sat);
KOMORI_DEFINE_SATURATED_ARITHMETIC_OPERATORS(*, mul_sat);
KOMORI_DEFINE_SATURATED_ARITHMETIC_OPERATORS(/, div_sat);

#undef KOMORI_DEFINE_SATURATED_ARITHMETIC_OPERATORS

template <typename T>
constexpr sat_t<T> operator-(sat_t<T> x) noexcept {
  return {neg_sat(static_cast<T>(x))};
}

template <typename T>
constexpr sat_t<T>& operator++(sat_t<T>& x) noexcept {
  return x += T{1};
}

template <typename T>
constexpr sat_t<T> operator++(sat_t<T>& x, int) noexcept {
  sat_t<T> tmp = x;
  ++x;
  return tmp;
}

template <typename T>
constexpr sat_t<T>& operator--(sat_t<T>& x) noexcept {
  return x -= T{1};
}

template <typename T>
constexpr sat_t<T> operator--(sat_t<T>& x, int) noexcept {
  sat_t<T> tmp = x;
  --x;
  return tmp;
}
}  // namespace detail

using int_sat8_t = detail::sat_t<std::int8_t>;
using uint_sat8_t = detail::sat_t<std::uint8_t>;
using int_sat16_t = detail::sat_t<std::int16_t>;
using uint_sat16_t = detail::sat_t<std::uint16_t>;
using int_sat32_t = detail::sat_t<std::int32_t>;
using uint_sat32_t = detail::sat_t<std::uint32_t>;
using int_sat64_t = detail::sat_t<std::int64_t>;
using uint_sat64_t = detail::sat_t<std::uint64_t>;
}  // namespace komori

namespace std {
template <typename T>
struct hash<komori::detail::sat_t<T>> {
  std::size_t operator()(const komori::detail::sat_t<T>& x) const noexcept { return std::hash<T>{}(x.value()); }
};
}  // namespace std

#undef KOMORI_HAS_BUILTIN
#undef KOMORI_CONSTEXPR_CPP17
#undef KOMORI_CONSTEXPR_CPP20

#endif  // KOMORI_OPERATIONS_HPP_
