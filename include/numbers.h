#pragma once

#include <cstdint>
#include <compare>
#include <iostream>
#include <limits>
#include <utility>
#include <array>

namespace Emulator {
    template<int N, bool is_fast>
    struct real_type {
        using type = std::conditional_t<N <= 8, int_fast8_t,
                std::conditional_t<N <= 16, int_fast16_t,
                        std::conditional_t<N <= 32, int_fast32_t,
                                std::conditional_t<N <= 64, int_fast64_t, void>>>>;
    };

    template<int N>
    struct real_type<N, false> {
        using type = std::conditional_t<N == 8, int8_t,
                std::conditional_t<N == 16, int16_t,
                        std::conditional_t<N == 32, int32_t,
                                std::conditional_t<N == 64, int64_t, void>>>>;
    };

    template<int N, bool is_fast>
    using real_type_t = typename real_type<N, is_fast>::type;

    template<int N, int K, bool fast = false> requires(K >= 0)
    struct Fixed {
        using real_t = real_type_t<N, fast>;
        static const int n = N;
        static const int k = K;

        real_t v;

        template<int N1, int K1, bool is_fast1>
        requires(K1 > K)
        constexpr Fixed(const Fixed<N1, K1, is_fast1> &other) : v(other.v >> (K1 - K)) {}

        template<int N1, int K1, bool is_fast1>
        requires(K1 <= K)
        constexpr Fixed(const Fixed<N1, K1, is_fast1> &other) : v(other.v << (K - K1)) {}

        constexpr Fixed(int64_t v) : v(v << K) {}

        constexpr Fixed(float f) : v(f * (1LL << K)) {}

        constexpr Fixed(double f) : v(f * (1LL << K)) {}

        constexpr Fixed() : v(0) {}

        explicit constexpr operator float() { return float(v) / (1LL << K); }

        explicit constexpr operator double() { return double(v) / (1LL << K); }

        static constexpr Fixed from_raw(int x) {
            Fixed ret{};
            ret.v = x;
            return ret;
        }

        auto operator<=>(const Fixed &) const = default;

        bool operator==(const Fixed &) const = default;

        friend Fixed operator+(Fixed a, Fixed b) {
            return Fixed::from_raw(a.v + b.v);
        }

        friend Fixed operator-(Fixed a, Fixed b) {
            return Fixed::from_raw(a.v - b.v);
        }

        friend Fixed operator*(Fixed a, Fixed b) {
            return Fixed::from_raw(((int64_t) a.v * b.v) >> K);
        }

        friend Fixed operator/(Fixed a, Fixed b) {
            return Fixed::from_raw(((int64_t) a.v << K) / b.v);
        }

        friend Fixed &operator+=(Fixed &a, Fixed b) {
            return a = a + b;
        }

        friend Fixed &operator-=(Fixed &a, Fixed b) {
            return a = a - b;
        }

        friend Fixed &operator*=(Fixed &a, Fixed b) {
            return a = a * b;
        }

        friend Fixed &operator/=(Fixed &a, Fixed b) {
            return a = a / b;
        }

        friend Fixed operator-(Fixed x) {
            return Fixed::from_raw(-x.v);
        }

        friend Fixed fabs(Fixed x) {
            if (x.v < 0) {
                x.v = -x.v;
            }
            return x;
        }

        friend std::ostream &operator<<(std::ostream &out, Fixed x) {
            return out << x.v / (double) (1 << K);
        }
    };
}