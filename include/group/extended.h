#ifndef JUBJUB_EXTENDED_H
#define JUBJUB_EXTENDED_H

#include <array>
#include <tuple>

#include "scalar/scalar.h"

namespace jubjub::field { class Fr; }

namespace jubjub::group {

class Affine;
class AffineNiels;
class Completed;
class ExtendedNiels;

class Extended {
private:
    bls12_381::scalar::Scalar x;
    bls12_381::scalar::Scalar y;
    bls12_381::scalar::Scalar z;

    bls12_381::scalar::Scalar t1;
    bls12_381::scalar::Scalar t2;

public:
    Extended();
    Extended(const Extended &extended);
    Extended(Extended &&extended) noexcept;

    explicit Extended(const Affine &affine);
    explicit Extended(Affine &&affine) noexcept;
    explicit Extended(const Completed &completed);

    Extended(bls12_381::scalar::Scalar x, bls12_381::scalar::Scalar y, bls12_381::scalar::Scalar z,
             bls12_381::scalar::Scalar t1, bls12_381::scalar::Scalar t2);

public:
    static Extended identity() noexcept;

    [[nodiscard]] bool is_identity() const;
    [[nodiscard]] bool is_small_order() const;
    [[nodiscard]] bool is_torsion_free() const;
    [[nodiscard]] bool is_prime_order() const;
    [[nodiscard]] bool is_on_curve() const;

    [[nodiscard]] std::tuple<bls12_381::scalar::Scalar, bls12_381::scalar::Scalar> to_hash_inputs() const;

    [[nodiscard]] Extended mul_by_cofactor() const;
    [[nodiscard]] Extended doubles() const;
    [[nodiscard]] Extended multiply(const std::array<uint8_t, 32> &by) const;

    [[nodiscard]] bls12_381::scalar::Scalar get_x() const;
    [[nodiscard]] bls12_381::scalar::Scalar get_y() const;
    [[nodiscard]] bls12_381::scalar::Scalar get_z() const;
    [[nodiscard]] bls12_381::scalar::Scalar get_t1() const;
    [[nodiscard]] bls12_381::scalar::Scalar get_t2() const;

public:
    Extended operator-() const;
    Extended &operator=(const Extended &rhs);
    Extended &operator=(Extended &&rhs) noexcept;

    Extended &operator+=(const Extended &rhs);
    Extended &operator-=(const Extended &rhs);
    Extended &operator+=(const ExtendedNiels &rhs);
    Extended &operator-=(const ExtendedNiels &rhs);

    Extended &operator+=(const Affine &rhs);
    Extended &operator-=(const Affine &rhs);
    Extended &operator+=(const AffineNiels &rhs);
    Extended &operator-=(const AffineNiels &rhs);

    Extended &operator*=(const field::Fr &rhs);

public:
    friend Extended operator+(const Extended &lhs, const Extended &rhs) { return Extended{lhs} += rhs; }
    friend Extended operator-(const Extended &lhs, const Extended &rhs) { return Extended{lhs} -= rhs; }

    friend Extended operator+(const Extended &lhs, const ExtendedNiels &rhs) { return Extended{lhs} += rhs; }
    friend Extended operator-(const Extended &lhs, const ExtendedNiels &rhs) { return Extended{lhs} -= rhs; }

    friend Extended operator+(const Extended &lhs, const Affine &rhs) { return Extended{lhs} += rhs; }
    friend Extended operator-(const Extended &lhs, const Affine &rhs) { return Extended{lhs} -= rhs; }

    friend Extended operator+(const Extended &lhs, const AffineNiels &rhs) { return Extended{lhs} += rhs; }
    friend Extended operator-(const Extended &lhs, const AffineNiels &rhs) { return Extended{lhs} -= rhs; }

    friend Extended operator*(const Extended &lhs, const field::Fr &rhs) { return Extended{lhs} *= rhs; }

    friend inline bool operator==(const Extended &lhs, const Extended &rhs) {
        return (lhs.x * rhs.z == rhs.x * lhs.z) && (lhs.y * rhs.z == rhs.y * lhs.z);
    }
    friend inline bool operator!=(const Extended &lhs, const Extended &rhs) {
        return (lhs.x * rhs.z != rhs.x * lhs.z) || (lhs.y * rhs.z != rhs.y * lhs.z);
    }
};

} // namespace jubjub::group

#endif //JUBJUB_EXTENDED_H