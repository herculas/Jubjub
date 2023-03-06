#ifndef JUBJUB_AFFINE_NIELS_H
#define JUBJUB_AFFINE_NIELS_H

#include <array>

#include "scalar/scalar.h"

namespace jubjub::field { class Fr; }

namespace jubjub::group {

class Affine;
class Extended;

class AffineNiels {
private:
    bls12_381::scalar::Scalar y_plus_x;
    bls12_381::scalar::Scalar y_minus_x;
    bls12_381::scalar::Scalar t2d;

public:
    AffineNiels();
    AffineNiels(const AffineNiels &point);
    AffineNiels(AffineNiels &&point) noexcept;

    explicit AffineNiels(const Affine &affine);

    static AffineNiels identity() noexcept;

    [[nodiscard]] Extended multiply(const std::array<uint8_t, 32> &by) const;

    [[nodiscard]] bls12_381::scalar::Scalar get_y_plus_x() const;
    [[nodiscard]] bls12_381::scalar::Scalar get_y_minus_x() const;
    [[nodiscard]] bls12_381::scalar::Scalar get_t2d() const;

public:
    friend Extended operator+(const AffineNiels &lhs, const Extended &rhs);
    friend Extended operator-(const AffineNiels &lhs, const Extended &rhs);

    friend Extended operator*(const AffineNiels &lhs, const field::Fr &rhs);
};

} // namespace jubjub::group

#endif //JUBJUB_AFFINE_NIELS_H