#ifndef JUBJUB_EXTENDED_NIELS_H
#define JUBJUB_EXTENDED_NIELS_H

#include "scalar/scalar.h"

namespace jubjub::field { class Fr; }

namespace jubjub::group {

class Extended;

class ExtendedNiels {
private:
    bls12_381::scalar::Scalar y_plus_x;
    bls12_381::scalar::Scalar y_minus_x;
    bls12_381::scalar::Scalar z;
    bls12_381::scalar::Scalar t2d;

public:
    ExtendedNiels();

    ExtendedNiels(const ExtendedNiels &extended);
    ExtendedNiels(ExtendedNiels &&extended) noexcept;

    explicit ExtendedNiels(const Extended &extended);

    static ExtendedNiels identity() noexcept;

    [[nodiscard]] Extended multiply(const std::array<uint8_t, 32> &by) const;

    [[nodiscard]] bls12_381::scalar::Scalar get_y_plus_x() const;
    [[nodiscard]] bls12_381::scalar::Scalar get_y_minus_x() const;
    [[nodiscard]] bls12_381::scalar::Scalar get_z() const;
    [[nodiscard]] bls12_381::scalar::Scalar get_t2d() const;

public:
    friend Extended operator+(const ExtendedNiels &lhs, const Extended &rhs);
    friend Extended operator-(const ExtendedNiels &lhs, const Extended &rhs);

    friend Extended operator*(const ExtendedNiels &lhs, const field::Fr &rhs);
};

} // namespace jubjub::group

#endif //JUBJUB_EXTENDED_NIELS_H