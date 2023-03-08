#include "group/affine.h"

#include "group/constants.h"
#include "group/extended.h"

namespace jubjub::group {

using bls12_381::scalar::Scalar;

using constant::EDWARDS_D1;

Affine::Affine() : x{Scalar::zero()}, y{Scalar::one()} {}

Affine::Affine(const Affine &affine) = default;

Affine::Affine(Affine &&affine) noexcept = default;

Affine::Affine(const Extended &extended) : x{extended.get_x()}, y{extended.get_y()} {
    const Scalar z_inv = extended.get_z().invert().value();
    this->x *= z_inv;
    this->y *= z_inv;
}

Affine::Affine(Scalar x, Scalar y) : x{std::move(x)}, y{std::move(y)} {}

Affine Affine::identity() noexcept {
    return Affine{};
}

std::optional<Affine> Affine::from_bytes(const std::array<uint8_t, Scalar::BYTE_SIZE> &bytes) {
    auto b = bytes;
    uint8_t sign = b[31] >> 7;
    b[31] &= 0b01111111;

    const auto y_temp = Scalar::from_bytes(b);
    if (!y_temp.has_value()) return std::nullopt;

    const Scalar &y = y_temp.value();
    const Scalar y2 = y.square();

    Scalar x = y2 - Scalar::one();
    const auto source = (Scalar::one() + EDWARDS_D1 * y2).invert();
    if (source.has_value())
        x *= source.value();
    else
        x *= Scalar::zero();

    if (!x.sqrt().has_value())
        return std::nullopt;
    x = x.sqrt().value();

    bool flip_sign = (x.to_bytes()[0] ^ sign) & 1;
    if (flip_sign) x = -x;
    bool x_is_zero = x.is_zero();

    if (x_is_zero & flip_sign)
        return std::nullopt;

    return Affine{x, y};
}

std::array<uint8_t, Scalar::BYTE_SIZE> Affine::to_bytes() const {
    const std::array<uint8_t, 32> x_bytes = this->x.to_bytes();
    std::array<uint8_t, 32> res = this->y.to_bytes();
    res[31] |= x_bytes[0] << 7;
    return res;
}

bool Affine::is_identity() const {
    return this->x == Scalar::zero() && this->y == Scalar::one();
}

bool Affine::is_small_order() const {
    return Extended{*this}.is_small_order();
}

bool Affine::is_torsion_free() const {
    return Extended{*this}.is_torsion_free();
}

bool Affine::is_prime_order() const {
    const Extended extended{*this};
    return extended.is_torsion_free() && (!extended.is_identity());
}

bool Affine::is_on_curve() const {
    const Scalar x2 = this->x.square();
    const Scalar y2 = this->y.square();
    return (y2 - x2 == Scalar::one() + EDWARDS_D1 * x2 * y2);
}

Extended Affine::mul_by_cofactor() const {
    return Extended{*this}.mul_by_cofactor();
}

bls12_381::scalar::Scalar Affine::get_x() const {
    return this->x;
}

bls12_381::scalar::Scalar Affine::get_y() const {
    return this->y;
}

Affine Affine::operator-() const {
    return Affine{-this->x, this->y};
}

Affine &Affine::operator=(const Affine &rhs) = default;

Affine &Affine::operator=(Affine &&rhs) noexcept = default;

Extended operator+(const Affine &lhs, const Extended &rhs) {
    return Extended{rhs} += lhs;
}

Extended operator-(const Affine &lhs, const Extended &rhs) {
    return Extended{rhs} -= lhs;
}

} // namespace jubjub::group