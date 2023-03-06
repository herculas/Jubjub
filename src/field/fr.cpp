#include "field/fr.h"

#include <cassert>
#include <cmath>

#include "field/constant.h"
#include "group/extended.h"

#include "utils/bit.h"
#include "utils/arith.h"

namespace jubjub::field {

using group::Extended;

using rng::util::bit::from_le_bytes;
using rng::util::bit::to_le_bytes;

using bls12_381::scalar::Scalar;
using bls12_381::util::arithmetic::adc;
using bls12_381::util::arithmetic::sbb;
using bls12_381::util::arithmetic::mac;

Fr::Fr() : data{0} {}

Fr::Fr(const Fr &fr) = default;

Fr::Fr(int8_t value) : data{{static_cast<uint64_t>(std::abs(value)), 0, 0, 0}} {
    if (value < 0) *this = -(*this);
}

Fr::Fr(uint64_t value) : data{{value, 0, 0, 0}} {
    *this *= constant::R2;
}

Fr::Fr(const std::array<uint64_t, Fr::WIDTH> &data) : data{data} {}

Fr::Fr(Fr &&fr) noexcept = default;

Fr::Fr(std::array<uint64_t, Fr::WIDTH> &&data) noexcept: data{data} {}

Fr Fr::zero() noexcept {
    return Fr{};
}

Fr Fr::one() noexcept {
    return constant::R1;
}

Fr Fr::random(rng::core::RngCore &rng) {
    std::array<uint8_t, Fr::BYTE_SIZE * 2> bytes{};
    rng.fill_bytes(bytes);
    return Fr::from_bytes_wide(bytes);
}

Fr Fr::montgomery_reduce(const std::array<uint64_t, Fr::WIDTH * 2> &ts) {
    uint64_t k;
    uint64_t carry = 0;
    uint64_t carry2 = 0;

    std::array<uint64_t, Fr::WIDTH * 2> r{};
    for (int i = 0; i < Fr::WIDTH * 2; ++i)
        r[i] = i < Fr::WIDTH ? ts[i] : 0;

    for (int i = 0; i < Fr::WIDTH; ++i) {
        k = r[i] * constant::INV;
        carry = 0;
        r[0] = mac(r[i], k, constant::MODULUS.data[0], carry);
        for (int j = 1; j < Fr::WIDTH; ++j)
            r[i + j] = mac(r[i + j], k, constant::MODULUS.data[j], carry);
        r[i + Fr::WIDTH] = adc(ts[i + Fr::WIDTH], carry2, carry);
        carry2 = carry;
    }

    return Fr({r[4], r[5], r[6], r[7]}) - constant::MODULUS;
}

Fr Fr::from_raw(const std::array<uint64_t, Fr::WIDTH> &values) {
    return Fr{values} * constant::R2;
}

Fr Fr::from_bytes_wide(const std::array<uint8_t, Fr::BYTE_SIZE * 2> &bytes) {
    std::array<std::array<uint8_t, sizeof(uint64_t)>, Fr::WIDTH * 2> array{};
    std::array<uint64_t, Fr::WIDTH * 2> data{};

    for (int i = 0; i < bytes.size(); ++i)
        array[i / sizeof(uint64_t)][i % sizeof(uint64_t)] = bytes[i];
    for (int i = 0; i < data.size(); ++i)
        data[i] = from_le_bytes<uint64_t>(array[i]);

    return Fr::reduce(data);
}

std::optional<Fr> Fr::from_bytes(const std::array<uint8_t, Fr::BYTE_SIZE> &bytes) {
    std::array<std::array<uint8_t, sizeof(uint64_t)>, Fr::WIDTH> array{};
    std::array<uint64_t, Fr::WIDTH> data{};

    for (int i = 0; i < bytes.size(); ++i)
        array[i / sizeof(uint64_t)][i % sizeof(uint64_t)] = bytes[i];
    for (int i = 0; i < data.size(); ++i)
        data[i] = rng::util::bit::from_le_bytes<uint64_t>(array[i]);

    Fr temp({data[0], data[1], data[2], data[3]});

    uint64_t borrow = 0;
    for (int i = 0; i < Fr::WIDTH; ++i)
        sbb(temp.data[i], constant::MODULUS.data[i], borrow);

    const uint8_t is_some = static_cast<uint8_t>(borrow) & 1;
    temp *= constant::R2;

    if (is_some) {
        return temp;
    } else {
        return std::nullopt;
    }
}

bool Fr::is_even() const {
    return this->data[0] % 2 == 0;
}

bool Fr::is_zero() const {
    return this->data[0] == 0 && this->data[1] == 0
           && this->data[2] == 0 && this->data[3] == 0;
}

std::optional<Scalar> Fr::to_bls_scalar() const {
    return Scalar::from_bytes(this->to_bytes());
}

std::array<uint8_t, Fr::BYTE_SIZE> Fr::to_bytes() const {
    std::array<uint64_t, Fr::WIDTH * 2> contents{0};
    std::copy(this->data.begin(), this->data.begin() + Fr::WIDTH, contents.begin());

    const Fr point = Fr::montgomery_reduce(contents);

    std::array<uint8_t, sizeof(uint64_t)> temp{};
    std::array<uint8_t, Fr::BYTE_SIZE> bytes{0};

    for (int i = 0; i < Fr::WIDTH; ++i) {
        temp = to_le_bytes<uint64_t>(point.data[i]);
        for (int j = 0; j < sizeof(uint64_t); ++j)
            bytes[i * 8 + j] = temp[j];
    }

    return bytes;
}

Fr Fr::doubles() const {
    return *this + *this;
}

Fr Fr::square() const {
    uint64_t carry = 0;
    std::array<uint64_t, Fr::WIDTH * 2> r{0};

    for (int i = 0; i < Fr::WIDTH - 1; ++i) {
        carry = 0;
        for (int j = 0; j < Fr::WIDTH - i - 2; ++j) {
            int32_t anchor = i * 2 + j + 1;
            r[anchor] = mac(r[anchor], this->data[i], this->data[i + j + 1], carry);
        }
        r[i + Fr::WIDTH - 1] = mac(r[i + Fr::WIDTH - 1], this->data[i], this->data[Fr::WIDTH - 1], carry);
        r[i + Fr::WIDTH] = carry;
    }

    r[7] = r[6] >> 63;
    for (int i = 6; i >= 2; --i) r[i] = (r[i] << 1) | (r[i - 1] >> 63);
    r[1] = r[1] << 1;

    carry = 0;
    r[0] = 0;
    for (int i = 0; i < Fr::WIDTH * 2; ++i) {
        if (i % 2 == 0) {
            r[i] = mac(r[i], this->data[i / 2], this->data[i / 2], carry);
        } else {
            r[i] = adc(r[i], 0, carry);
        }
    }

    return Fr::montgomery_reduce(r);
}

Fr Fr::pow(const std::array<uint64_t, Fr::WIDTH> &exp) const {
    Fr res = Fr::one();
    for (int i = Fr::WIDTH - 1; i >= 0; --i) {
        for (int j = 63; j >= 0; --j) {
            res = res.square();
            if (((exp[i] >> j) & 0x01) == 0x01) res *= *this;
        }
    }
    return res;
}

Fr Fr::self_reduce() const {
    return Fr::montgomery_reduce({this->data[0], this->data[1], this->data[2], this->data[3], 0, 0, 0, 0});
}

void Fr::div_n(uint32_t n) {
    if (n >= 256) {
        *this = Fr{64ULL};
        return;
    }

    while (n >= 64) {
        uint64_t t = 0;
        for (int i = Fr::WIDTH - 1; i >= 0; --i)
            std::swap(t, this->data[i]);
        n -= 64;
    }

    if (n > 0) {
        uint64_t t = 0;
        for (int i = Fr::WIDTH - 1; i >= 0; --i) {
            const uint64_t t2 = this->data[i] << (64 - n);
            this->data[i] >>= n;
            this->data[i] |= t;
            t = t2;
        }
    }
}

std::optional<Fr> Fr::sqrt() const {
    const Fr sqrt = this->pow(
            {
                    0xb425c397b5bdcb2e, 0x299a0824f3320420,
                    0x4199cec0404d0ec0, 0x039f6d3a994cebea,
            }
    );
    if (sqrt.square() == *this)
        return sqrt;
    else
        return std::nullopt;
}

std::optional<Fr> Fr::invert() const {
    const auto square_assign_multi = [](Fr &n, size_t num_times) {
        for (int i = 0; i < num_times; ++i) n = n.square();
    };

    Fr t1 = this->square();
    Fr t0 = t1.square();
    Fr t3 = t0 * t1;
    const Fr t6 = t3 * *this;
    const Fr t7 = t6 * t1;
    const Fr t12 = t7 * t3;
    const Fr t13 = t12 * t0;
    const Fr t16 = t12 * t3;
    const Fr t2 = t13 * t3;
    const Fr t15 = t16 * t3;
    const Fr t19 = t2 * t0;
    const Fr t9 = t15 * t3;
    const Fr t18 = t9 * t3;
    const Fr t14 = t18 * t1;
    const Fr t4 = t18 * t0;
    const Fr t8 = t18 * t3;
    const Fr t17 = t14 * t3;
    const Fr t11 = t8 * t3;
    t1 = t17 * t3;
    const Fr t5 = t11 * t3;
    t3 = t5 * t0;
    t0 = t5.square();

    square_assign_multi(t0, 5);
    t0 *= t3;
    square_assign_multi(t0, 6);
    t0 *= t8;
    square_assign_multi(t0, 7);
    t0 *= t19;
    square_assign_multi(t0, 6);
    t0 *= t13;
    square_assign_multi(t0, 8);
    t0 *= t14;
    square_assign_multi(t0, 6);
    t0 *= t18;
    square_assign_multi(t0, 7);
    t0 *= t17;
    square_assign_multi(t0, 5);
    t0 *= t16;
    square_assign_multi(t0, 3);
    t0 *= *this;
    square_assign_multi(t0, 11);
    t0 *= t11;
    square_assign_multi(t0, 8);
    t0 *= t5;
    square_assign_multi(t0, 5);
    t0 *= t15;
    square_assign_multi(t0, 8);
    t0 *= *this;
    square_assign_multi(t0, 12);
    t0 *= t13;
    square_assign_multi(t0, 7);
    t0 *= t9;
    square_assign_multi(t0, 5);
    t0 *= t15;
    square_assign_multi(t0, 14);
    t0 *= t14;
    square_assign_multi(t0, 5);
    t0 *= t13;
    square_assign_multi(t0, 2);
    t0 *= *this;
    square_assign_multi(t0, 6);
    t0 *= *this;
    square_assign_multi(t0, 9);
    t0 *= t7;
    square_assign_multi(t0, 6);
    t0 *= t12;
    square_assign_multi(t0, 8);
    t0 *= t11;
    square_assign_multi(t0, 3);
    t0 *= *this;
    square_assign_multi(t0, 12);
    t0 *= t9;
    square_assign_multi(t0, 11);
    t0 *= t8;
    square_assign_multi(t0, 8);
    t0 *= t7;
    square_assign_multi(t0, 4);
    t0 *= t6;
    square_assign_multi(t0, 10);
    t0 *= t5;
    square_assign_multi(t0, 7);
    t0 *= t3;
    square_assign_multi(t0, 6);
    t0 *= t4;
    square_assign_multi(t0, 7);
    t0 *= t3;
    square_assign_multi(t0, 5);
    t0 *= t2;
    square_assign_multi(t0, 6);
    t0 *= t2;
    square_assign_multi(t0, 7);
    t0 *= t1;

    if (this->is_zero())
        return std::nullopt;
    else
        return t0;
}

uint8_t Fr::mod_2_pow_k(uint8_t k) const {
    return static_cast<uint8_t>(this->data[0] & ((1 << k) - 1));
}

int8_t Fr::mod_k(uint8_t w) const {
    assert(w < 32);
    const auto modulus = static_cast<int8_t>(this->mod_2_pow_k(w));
    const int8_t two_pow_w_minus_one = static_cast<int8_t>(1) << (w - 1); // NOLINT(cppcoreguidelines-narrowing-conversions)
    return modulus >= two_pow_w_minus_one ? modulus - static_cast<int8_t>((static_cast<int8_t>(1) << w)) : modulus; // NOLINT(cppcoreguidelines-narrowing-conversions)
}

std::array<int8_t, 256> Fr::compute_windowed_non_adjacent(uint8_t width) const {
    Fr k = this->self_reduce();
    size_t i = 0;
    Fr one = Fr::one().self_reduce();
    std::array<int8_t, 256> res{};

    while (k >= one) {
        if (!k.is_even()) {
            const int8_t ki = k.mod_k(width);
            res[i] = ki;
            k = k - Fr{ki};
        } else {
            res[i] = 0;
        }

        k.div_n(1);
        i += 1;
    }
    return res;
}

Fr Fr::reduce(const std::array<uint64_t, Fr::WIDTH * 2> &limbs) {
    const Fr d0({limbs[0], limbs[1], limbs[2], limbs[3]});
    const Fr d1({limbs[4], limbs[5], limbs[6], limbs[7]});
    return d0 * constant::R2 + d1 * constant::R3;
}

Fr Fr::operator-() const {
    uint64_t borrow = 0;
    uint64_t d[Fr::WIDTH];

    for (int i = 0; i < Fr::WIDTH; ++i)
        d[i] = sbb(constant::MODULUS.data[i], this->data[i], borrow);

    bool dec = (this->data[0] | this->data[1] | this->data[2] | this->data[3]) == 0;
    uint64_t mask = static_cast<uint64_t>(dec) - 1;

    return Fr(
            {
                    d[0] & mask, d[1] & mask,
                    d[2] & mask, d[3] & mask,
            }
    );
}

Fr &Fr::operator=(const Fr &rhs) = default;

Fr &Fr::operator=(Fr &&rhs) noexcept = default;

Fr &Fr::operator+=(const Fr &rhs) {
    uint64_t carry = 0;
    uint64_t d[Fr::WIDTH];
    for (int i = 0; i < Fr::WIDTH; ++i)
        d[i] = adc(this->data[i], rhs.data[i], carry);
    *this = Fr({d[0], d[1], d[2], d[3]}) - constant::MODULUS;
    return *this;
}

Fr &Fr::operator-=(const Fr &rhs) {
    uint64_t borrow = 0;
    uint64_t d[Fr::WIDTH];
    for (int i = 0; i < Fr::WIDTH; ++i)
        d[i] = sbb(this->data[i], rhs.data[i], borrow);
    uint64_t carry = 0;
    for (int i = 0; i < Fr::WIDTH; ++i)
        d[i] = adc(d[i], constant::MODULUS.data[i] & borrow, carry);
    *this = Fr({d[0], d[1], d[2], d[3]});
    return *this;
}

Fr &Fr::operator*=(const Fr &rhs) {
    uint64_t carry = 0;
    std::array<uint64_t, Fr::WIDTH * 2> r{0};

    for (int i = 0; i < Fr::WIDTH; ++i) {
        carry = 0;
        for (int j = 0; j < Fr::WIDTH - 1; ++j)
            r[i + j] = mac(i == 0 ? 0 : r[i + j], this->data[i], rhs.data[j], carry);
        r[i + Fr::WIDTH - 1] = mac(i == 0 ? 0 : r[i + Fr::WIDTH - 1], this->data[i], rhs.data[Fr::WIDTH - 1], carry);
        r[i + Fr::WIDTH] = carry;
    }

    *this = Fr::montgomery_reduce(r);
    return *this;
}

std::strong_ordering operator<=>(const Fr &lhs, const Fr &rhs) {
    for (int i = 3; i >= 0; --i) {
        if (lhs.data[i] > rhs.data[i])
            return std::strong_ordering::greater;
        else if (lhs.data[i] < rhs.data[i])
            return std::strong_ordering::less;
    }
    return std::strong_ordering::equivalent;
}

Extended operator*(const Fr &lhs, const Extended &rhs) {
    return Extended{rhs} *= lhs;
}

} // namespace jubjub::field