#ifndef JUBJUB_AFFINE_H
#define JUBJUB_AFFINE_H

#include <optional>

#include "scalar/scalar.h"

namespace jubjub::group {

class AffineNiels;
class Extended;

class Affine {
private:
    bls12_381::scalar::Scalar x;
    bls12_381::scalar::Scalar y;

public:
    Affine();
    Affine(const Affine &affine);
    Affine(Affine &&affine) noexcept;

    explicit Affine(const Extended &extended);

    Affine(bls12_381::scalar::Scalar x, bls12_381::scalar::Scalar y);

    static Affine identity() noexcept;

    static std::optional<Affine> from_bytes(const std::array<uint8_t, bls12_381::scalar::Scalar::BYTE_SIZE> &bytes);

    [[nodiscard]] std::array<uint8_t, bls12_381::scalar::Scalar::BYTE_SIZE> to_bytes() const;

    [[nodiscard]] bool is_identity() const;
    [[nodiscard]] bool is_small_order() const;
    [[nodiscard]] bool is_torsion_free() const;
    [[nodiscard]] bool is_prime_order() const;
    [[nodiscard]] bool is_on_curve() const;

    [[nodiscard]] Extended mul_by_cofactor() const;

    [[nodiscard]] bls12_381::scalar::Scalar get_x() const;
    [[nodiscard]] bls12_381::scalar::Scalar get_y() const;

public:
    Affine operator-() const;
    Affine &operator=(const Affine &rhs);
    Affine &operator=(Affine &&rhs) noexcept;

public:
    friend Extended operator+(const Affine &lhs, const Extended &rhs);
    friend Extended operator-(const Affine &lhs, const Extended &rhs);

    friend inline bool operator==(const Affine &lhs, const Affine &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
    friend inline bool operator!=(const Affine &lhs, const Affine &rhs) { return lhs.x != rhs.x || lhs.y != rhs.y; }


};

} // namespace jubjub::group

#endif //JUBJUB_AFFINE_H