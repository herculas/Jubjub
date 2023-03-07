#ifndef JUBJUB_CIPHER_H
#define JUBJUB_CIPHER_H

#include <array>
#include <optional>

#include "field/fr.h"
#include "group/extended.h"

namespace jubjub::elgamal {

class Cipher {
public:
    static constexpr int32_t BYTE_SIZE = 64;
private:
    group::Extended gamma;
    group::Extended delta;

public:
    Cipher();
    Cipher(const Cipher &cipher);
    Cipher(Cipher &&cipher) noexcept;

    Cipher(group::Extended gamma, group::Extended delta);

    static std::optional<Cipher> from_bytes(const std::array<uint8_t, Cipher::BYTE_SIZE> &bytes);
    static Cipher encrypt(const field::Fr &sec, const group::Extended &pub, const group::Extended &gen, const group::Extended &msg);

    [[nodiscard]] std::array<uint8_t, Cipher::BYTE_SIZE> to_bytes() const;
    [[nodiscard]] group::Extended decrypt(const field::Fr &sec) const;

    [[nodiscard]] group::Extended get_gamma() const;
    [[nodiscard]] group::Extended get_delta() const;

public:
    Cipher &operator=(const Cipher &rhs);
    Cipher &operator=(Cipher &&rhs) noexcept;

    Cipher &operator+=(const Cipher &rhs);
    Cipher &operator-=(const Cipher &rhs);
    Cipher &operator*=(const field::Fr &rhs);

public:
    friend inline Cipher operator+(const Cipher &lhs, const Cipher &rhs) { return Cipher{lhs} += rhs; }
    friend inline Cipher operator-(const Cipher &lhs, const Cipher &rhs) { return Cipher{lhs} -= rhs; }
    friend inline Cipher operator*(const Cipher &lhs, const field::Fr &rhs) { return Cipher{lhs} *= rhs; }
};

} // namespace jubjub::elgamal

#endif //JUBJUB_CIPHER_H