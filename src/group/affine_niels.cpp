#include "group/affine_niels.h"

#include "field/fr.h"

#include "group/affine.h"
#include "group/extended.h"
#include "group/constant.h"

namespace jubjub::group {

using bls12_381::scalar::Scalar;

using field::Fr;
using constant::EDWARDS_D2;

AffineNiels::AffineNiels() : y_plus_x{Scalar::one()}, y_minus_x{Scalar::one()}, t2d{Scalar::zero()} {}

AffineNiels::AffineNiels(const AffineNiels &point) = default;

AffineNiels::AffineNiels(AffineNiels &&point) noexcept = default;

AffineNiels::AffineNiels(const Affine &affine)
        : y_plus_x{affine.get_y() + affine.get_x()}, y_minus_x{affine.get_y() - affine.get_x()},
          t2d{affine.get_x() * affine.get_y() * EDWARDS_D2} {}

AffineNiels AffineNiels::identity() noexcept {
    return AffineNiels{};
}

Extended AffineNiels::multiply(const std::array<uint8_t, 32> &by) const {
    const AffineNiels zero = AffineNiels::identity();
    Extended acc = Extended::identity();

    for (auto iter = by.rbegin(); iter != by.rend(); iter++) {
        for (int i = 7; i >= 0; --i) {
            if (iter == by.rbegin() && i > 3) continue;
            bool bit = (*iter >> i) & 1;
            acc = acc.doubles();
            if (bit) {
                acc += *this;
            } else {
                acc += zero;
            }
        }
    }
    return acc;
}

bls12_381::scalar::Scalar AffineNiels::get_y_plus_x() const {
    return this->y_plus_x;
}

bls12_381::scalar::Scalar AffineNiels::get_y_minus_x() const {
    return this->y_minus_x;
}

bls12_381::scalar::Scalar AffineNiels::get_t2d() const {
    return this->t2d;
}

Extended operator+(const AffineNiels &lhs, const Extended &rhs) {
    return Extended{rhs} += lhs;
}

Extended operator-(const AffineNiels &lhs, const Extended &rhs) {
    return Extended{rhs} -= lhs;
}

Extended operator*(const AffineNiels &lhs, const Fr &rhs) {
    return Extended{lhs.multiply(rhs.to_bytes())};
}

} // namespace jubjub::group