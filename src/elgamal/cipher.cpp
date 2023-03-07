#include "elgamal/cipher.h"

#include "group/affine.h"

namespace jubjub::elgamal {

using field::Fr;
using group::Affine;
using group::Extended;

Cipher::Cipher() = default;

Cipher::Cipher(const Cipher &cipher) = default;

Cipher::Cipher(Cipher &&cipher) noexcept = default;

Cipher::Cipher(Extended gamma, Extended delta) : gamma{std::move(gamma)}, delta{std::move(delta)} {}

std::optional<Cipher> Cipher::from_bytes(const std::array<uint8_t, Cipher::BYTE_SIZE> &bytes) {
    std::array<uint8_t, 32> bytes_gamma{};
    std::array<uint8_t, 32> bytes_delta{};
    std::copy(bytes.begin(), bytes.begin() + 32, bytes_gamma.begin());
    std::copy(bytes.begin() + 32, bytes.end(), bytes_delta.begin());

    const auto gamma_opt = Affine::from_bytes(bytes_gamma);
    const auto delta_opt = Affine::from_bytes(bytes_delta);

    if (!gamma_opt.has_value() || !delta_opt.has_value()) return std::nullopt;
    return Cipher{Extended{gamma_opt.value()}, Extended{delta_opt.value()}};
}

Cipher Cipher::encrypt(const Fr &sec, const Extended &pub, const Extended &gen, const Extended &msg) {
    const Extended gamma_extended = gen * sec;
    const Extended delta_extended = msg + pub * sec;
    return Cipher{gamma_extended, delta_extended};
}

std::array<uint8_t, Cipher::BYTE_SIZE> Cipher::to_bytes() const {
    const Affine gamma_affine = Affine{this->gamma};
    const Affine delta_affine = Affine{this->delta};

    const auto gamma_bytes = gamma_affine.to_bytes();
    const auto delta_bytes = delta_affine.to_bytes();

    std::array<uint8_t, Cipher::BYTE_SIZE> res{};

    std::copy(gamma_bytes.begin(), gamma_bytes.end(), res.begin());
    std::copy(delta_bytes.begin(), delta_bytes.end(), res.begin() + 32);

    return res;
}

group::Extended Cipher::decrypt(const field::Fr &sec) const {
    return this->delta - this->gamma * sec;
}

group::Extended Cipher::get_gamma() const {
    return this->gamma;
}

group::Extended Cipher::get_delta() const {
    return this->delta;
}

Cipher &Cipher::operator=(const Cipher &rhs) = default;

Cipher &Cipher::operator=(Cipher &&rhs) noexcept = default;

Cipher &Cipher::operator+=(const Cipher &rhs) {
    *this = Cipher{this->gamma + rhs.gamma, this->delta + rhs.delta};
    return *this;
}

Cipher &Cipher::operator-=(const Cipher &rhs) {
    *this = Cipher{this->gamma - rhs.gamma, this->delta - rhs.delta};
    return *this;
}

Cipher &Cipher::operator*=(const field::Fr &rhs) {
    *this = Cipher{this->gamma * rhs, this->delta * rhs};
    return *this;
}
} // namespace jubjub::elgamal