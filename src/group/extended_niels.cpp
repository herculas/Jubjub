#include "group/extended_niels.h"

#include "field/fr.h"

#include "group/extended.h"
#include "group/constant.h"

namespace jubjub::group {

using bls12_381::scalar::Scalar;

using field::Fr;
using constant::EDWARDS_D2;

ExtendedNiels::ExtendedNiels()
        : y_plus_x{Scalar::one()}, y_minus_x{Scalar::one()}, z{Scalar::one()}, t2d{Scalar::zero()} {}

ExtendedNiels ExtendedNiels::identity() noexcept {
    return ExtendedNiels{};
}

ExtendedNiels::ExtendedNiels(const ExtendedNiels &extended) = default;

ExtendedNiels::ExtendedNiels(ExtendedNiels &&extended) noexcept = default;

ExtendedNiels::ExtendedNiels(const Extended &extended)
        : y_plus_x{extended.get_y() + extended.get_x()}, y_minus_x{extended.get_y() - extended.get_x()},
          z{extended.get_z()}, t2d{extended.get_t1() * extended.get_t2() * EDWARDS_D2} {}

Extended ExtendedNiels::multiply(const std::array<uint8_t, 32> &by) const {
    const ExtendedNiels zero = ExtendedNiels::identity();
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

bls12_381::scalar::Scalar ExtendedNiels::get_y_plus_x() const {
    return this->y_plus_x;
}

bls12_381::scalar::Scalar ExtendedNiels::get_y_minus_x() const {
    return this->y_minus_x;
}

bls12_381::scalar::Scalar ExtendedNiels::get_z() const {
    return this->z;
}

bls12_381::scalar::Scalar ExtendedNiels::get_t2d() const {
    return this->t2d;
}

Extended operator+(const ExtendedNiels &lhs, const Extended &rhs) {
    return Extended{rhs} += lhs;
}

Extended operator-(const ExtendedNiels &lhs, const Extended &rhs) {
    return Extended{rhs} -= lhs;
}

Extended operator*(const ExtendedNiels &lhs, const Fr &rhs) {
    return Extended{lhs.multiply(rhs.to_bytes())};
}

} // namespace jubjub::group