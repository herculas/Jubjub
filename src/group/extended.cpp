#include "group/extended.h"

#include "field/fr.h"

#include "group/constants.h"
#include "group/affine.h"
#include "group/affine_niels.h"
#include "group/completed.h"
#include "group/extended_niels.h"

namespace jubjub::group {

using bls12_381::scalar::Scalar;
using constant::FR_MODULUS_BYTES;

Extended::Extended()
        : x{Scalar::zero()}, y{Scalar::one()}, z{Scalar::one()}, t1{Scalar::zero()}, t2{Scalar::zero()} {}

Extended::Extended(const Extended &extended) = default;

Extended::Extended(Extended &&extended) noexcept = default;

Extended::Extended(const Affine &affine)
        : x{affine.get_x()}, y{affine.get_y()}, z{Scalar::one()}, t1{affine.get_x()}, t2{affine.get_y()} {}

Extended::Extended(Affine &&affine) noexcept
        : x{affine.get_x()}, y{affine.get_y()}, z{Scalar::one()}, t1{affine.get_x()}, t2{affine.get_y()} {}

Extended::Extended(const Completed &completed)
        : x{completed.x * completed.t}, y{completed.y * completed.z}, z{completed.z * completed.t},
          t1{completed.x}, t2{completed.y} {}

Extended::Extended(bls12_381::scalar::Scalar x, bls12_381::scalar::Scalar y, bls12_381::scalar::Scalar z,
                   bls12_381::scalar::Scalar t1, bls12_381::scalar::Scalar t2)
        : x{std::move(x)}, y{std::move(y)}, z{std::move(z)}, t1{std::move(t1)}, t2{std::move(t2)} {}

Extended Extended::identity() noexcept {
    return Extended{};
}

bool Extended::is_identity() const {
    return (this->x == Scalar::zero()) && (this->y == this->z);
}

bool Extended::is_small_order() const {
    return this->doubles().doubles().x == Scalar::zero();
}

bool Extended::is_torsion_free() const {
    return this->multiply(FR_MODULUS_BYTES).is_identity();
}

bool Extended::is_prime_order() const {
    return this->is_torsion_free() && (!this->is_identity());
}

bool Extended::is_on_curve() const {
    const Affine affine{*this};
    return this->z != Scalar::zero()
           && affine.is_on_curve()
           && (affine.get_x() * affine.get_y() * this->z == this->t1 * this->t2);
}

std::tuple<Scalar, Scalar> Extended::to_hash_inputs() const {
    const Affine p{*this};
    return {p.get_x(), p.get_y()};
}

Extended Extended::mul_by_cofactor() const {
    return this->doubles().doubles().doubles();
}

Extended Extended::doubles() const {
    const Scalar xx = this->x.square();
    const Scalar yy = this->y.square();
    const Scalar zz2 = this->z.square().doubles();
    const Scalar xy2 = (this->x + this->y).square();
    const Scalar yy_plus_xx = yy + xx;
    const Scalar yy_minus_xx = yy - xx;
    const Completed completed{xy2 - yy_plus_xx, yy_plus_xx, yy_minus_xx, zz2 - yy_minus_xx};
    return Extended{completed};
}

Extended Extended::multiply(const std::array<uint8_t, 32> &by) const {
    return ExtendedNiels{*this}.multiply(by);
}

bls12_381::scalar::Scalar Extended::get_x() const {
    return this->x;
}

bls12_381::scalar::Scalar Extended::get_y() const {
    return this->y;
}

bls12_381::scalar::Scalar Extended::get_z() const {
    return this->z;
}

bls12_381::scalar::Scalar Extended::get_t1() const {
    return this->t1;
}

bls12_381::scalar::Scalar Extended::get_t2() const {
    return this->t2;
}

Extended Extended::operator-() const {
    return Extended{-this->x, this->y, this->z, -this->t1, this->t2};
}

Extended &Extended::operator=(const Extended &rhs) = default;

Extended &Extended::operator=(Extended &&rhs) noexcept = default;

Extended &Extended::operator+=(const AffineNiels &rhs) {
    const Scalar a = (this->y - this->x) * rhs.get_y_minus_x();
    const Scalar b = (this->y + this->x) * rhs.get_y_plus_x();
    const Scalar c = this->t1 * this->t2 * rhs.get_t2d();
    const Scalar d = this->z.doubles();
    *this = Extended{Completed{b - a, b + a, d + c, d - c}};
    return *this;
}

Extended &Extended::operator-=(const AffineNiels &rhs) {
    const Scalar a = (this->y - this->x) * rhs.get_y_plus_x();
    const Scalar b = (this->y + this->x) * rhs.get_y_minus_x();
    const Scalar c = this->t1 * this->t2 * rhs.get_t2d();
    const Scalar d = this->z.doubles();
    *this = Extended{Completed{b - a, b + a, d - c, d + c}};
    return *this;
}

Extended &Extended::operator+=(const Affine &rhs) {
    *this += AffineNiels{rhs};
    return *this;
}

Extended &Extended::operator-=(const Affine &rhs) {
    *this -= AffineNiels{rhs};
    return *this;
}

Extended &Extended::operator+=(const Extended &rhs) {
    *this += ExtendedNiels{rhs};
    return *this;
}

Extended &Extended::operator-=(const Extended &rhs) {
    *this -= ExtendedNiels{rhs};
    return *this;
}

Extended &Extended::operator+=(const ExtendedNiels &rhs) {
    const Scalar a = (this->y - this->x) * rhs.get_y_minus_x();
    const Scalar b = (this->y + this->x) * rhs.get_y_plus_x();
    const Scalar c = this->t1 * this->t2 * rhs.get_t2d();
    const Scalar d = (this->z * rhs.get_z()).doubles();

    const Completed cp{b - a, b + a, d + c, d - c};
    const Extended ext{cp};

    *this = ext;
    return *this;
}

Extended &Extended::operator-=(const ExtendedNiels &rhs) {
    const Scalar a = (this->y - this->x) * rhs.get_y_plus_x();
    const Scalar b = (this->y + this->x) * rhs.get_y_minus_x();
    const Scalar c = this->t1 * this->t2 * rhs.get_t2d();
    const Scalar d = (this->z * rhs.get_z()).doubles();
    *this = Extended{Completed{b - a, b + a, d - c, d + c}};
    return *this;
}

Extended &Extended::operator*=(const field::Fr &rhs) {
    *this = this->multiply(rhs.to_bytes());
    return *this;
}

void Extended::set_x(const Scalar &scalar) {
    this->x = scalar;
}

void Extended::set_y(const Scalar &scalar) {
    this->y = scalar;
}

void Extended::set_z(const Scalar &scalar) {
    this->z = scalar;
}

void Extended::set_t1(const Scalar &scalar) {
    this->t1 = scalar;
}

void Extended::set_t2(const Scalar &scalar) {
    this->t2 = scalar;
}

} // namespace jubjub::group