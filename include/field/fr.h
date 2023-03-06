#ifndef JUBJUB_FR_H
#define JUBJUB_FR_H

#include <array>
#include <cstdint>
#include <optional>

#include "core/rng.h"
#include "scalar/scalar.h"

namespace jubjub::group { class Extended; }

namespace jubjub::field {

class Fr {
public:
    static constexpr int32_t WIDTH = 4;
    static constexpr int32_t BYTE_SIZE = Fr::WIDTH * sizeof(uint64_t);

private:
    std::array<uint64_t, Fr::WIDTH> data;

public:
    Fr();
    Fr(const Fr &fr);
    explicit Fr(int8_t value);
    explicit Fr(uint64_t value);
    explicit Fr(const std::array<uint64_t, Fr::WIDTH> &data);

    Fr(Fr &&fr) noexcept;
    explicit Fr(std::array<uint64_t, Fr::WIDTH> &&data) noexcept;

    static Fr zero() noexcept;
    static Fr one() noexcept;
    static Fr random(rng::core::RngCore &rng);

    static Fr from_raw(const std::array<uint64_t, Fr::WIDTH> &values);
    static Fr montgomery_reduce(const std::array<uint64_t, Fr::WIDTH * 2> &ts);
    static Fr from_bytes_wide(const std::array<uint8_t, Fr::BYTE_SIZE * 2> &bytes);

    static std::optional<Fr> from_bytes(const std::array<uint8_t, Fr::BYTE_SIZE> &bytes);

    [[nodiscard]] bool is_even() const;
    [[nodiscard]] bool is_zero() const;
    [[nodiscard]] std::optional<bls12_381::scalar::Scalar> to_bls_scalar() const;
    [[nodiscard]] std::array<uint8_t, Fr::BYTE_SIZE> to_bytes() const;

    [[nodiscard]] Fr doubles() const;
    [[nodiscard]] Fr square() const;
    [[nodiscard]] Fr self_reduce() const;
    [[nodiscard]] Fr pow(const std::array<uint64_t, Fr::WIDTH> &exp) const;

    void div_n(uint32_t n);

    [[nodiscard]] std::optional<Fr> sqrt() const;
    [[nodiscard]] std::optional< Fr> invert() const;

    [[nodiscard]] uint8_t mod_2_pow_k(uint8_t k) const;
    [[nodiscard]] int8_t mod_k(uint8_t w) const;
    [[nodiscard]] std::array<int8_t, 256> compute_windowed_non_adjacent(uint8_t width) const;

private:
    static Fr reduce(const std::array<uint64_t, Fr::WIDTH * 2> &limbs);

public:
    Fr operator-() const;
    Fr &operator=(const Fr &rhs);
    Fr &operator=(Fr &&rhs) noexcept;

    Fr &operator+=(const Fr &rhs);
    Fr &operator-=(const Fr &rhs);
    Fr &operator*=(const Fr &rhs);

public:
    friend std::strong_ordering operator<=>(const Fr &lhs, const Fr &rhs);

    friend inline Fr operator+(const Fr &lhs, const Fr &rhs) { return Fr(lhs) += rhs; }
    friend inline Fr operator-(const Fr &lhs, const Fr &rhs) { return Fr(lhs) -= rhs; }
    friend inline Fr operator*(const Fr &lhs, const Fr &rhs) { return Fr(lhs) *= rhs; }

    friend inline bool operator==(const Fr &lhs, const Fr &rhs) { return lhs.data == rhs.data; }
    friend inline bool operator!=(const Fr &lhs, const Fr &rhs) { return lhs.data != rhs.data; }

    friend group::Extended operator*(const Fr &lhs, const group::Extended &rhs);
};

} // namespace jubjub::field

#endif //JUBJUB_FR_H