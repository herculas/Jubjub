#include <gtest/gtest.h>

#include <tuple>

#include "impl/os_rng.h"

#include "elgamal/cipher.h"
#include "field/fr.h"
#include "group/extended.h"
#include "group/constants.h"

using rng::impl::OsRng;
using jubjub::elgamal::Cipher;
using jubjub::field::Fr;
using jubjub::group::Extended;
using jubjub::group::constant::GENERATOR_EXTENDED;

std::tuple<Fr, Extended, Fr, Extended> generate() {
    OsRng rng{};
    const Fr a = Fr::random(rng);
    const Fr b = Fr::random(rng);

    const Extended a_g = GENERATOR_EXTENDED * a;
    const Extended b_g = GENERATOR_EXTENDED * b;

    return {a, a_g, b, b_g};
}

TEST(ElGamal, Enc) {
    auto [a, _, b, b_g] = generate();

    OsRng rng{};
    const Fr m = Fr::random(rng);
    const Extended m_g = GENERATOR_EXTENDED * m;

    const Cipher cipher = Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g);
    const Extended decrypt = cipher.decrypt(b);

    EXPECT_EQ(m_g, decrypt);
}

TEST(ElGamal, EncWrongKey) {
    auto [a, _, b, b_g] = generate();

    OsRng rng{};
    const Fr m = Fr::random(rng);
    const Extended m_g = GENERATOR_EXTENDED * m;

    const Cipher cipher = Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g);
    const Fr wrong_key = b - Fr::one();
    const Extended decrypt = cipher.decrypt(wrong_key);

    EXPECT_NE(m_g, decrypt);
}

TEST(ElGamal, HomomorphicAdd) {
    auto [a, _, b, b_g] = generate();

    OsRng rng{};
    const std::array<Fr, 4> m = {Fr::random(rng), Fr::random(rng), Fr::random(rng), Fr::random(rng)};
    const std::array<Extended, 4> m_g = {
            GENERATOR_EXTENDED * m[0], GENERATOR_EXTENDED * m[1],
            GENERATOR_EXTENDED * m[2], GENERATOR_EXTENDED * m[3]
    };

    const Fr res = m[0] + m[1] + m[2] + m[3];
    const Extended res_g = GENERATOR_EXTENDED * res;

    const std::array<Cipher, 4> cipher = {
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[0]),
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[1]),
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[2]),
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[3]),
    };
    Cipher homo_cipher = cipher[0] + cipher[1];
    homo_cipher += cipher[2];
    homo_cipher = homo_cipher + cipher[3];

    const Extended homo_dec = homo_cipher.decrypt(b);

    EXPECT_EQ(res_g, homo_dec);
}

TEST(ElGamal, HomomorphicSub) {
    auto [a, _, b, b_g] = generate();

    OsRng rng{};
    const std::array<Fr, 4> m = {Fr::random(rng), Fr::random(rng), Fr::random(rng), Fr::random(rng)};
    const std::array<Extended, 4> m_g = {
            GENERATOR_EXTENDED * m[0], GENERATOR_EXTENDED * m[1],
            GENERATOR_EXTENDED * m[2], GENERATOR_EXTENDED * m[3]
    };

    const Fr res = m[0] - m[1] - m[2] - m[3];
    const Extended res_g = GENERATOR_EXTENDED * res;

    const std::array<Cipher, 4> cipher = {
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[0]),
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[1]),
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[2]),
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[3]),
    };
    Cipher homo_cipher = cipher[0] - cipher[1];
    homo_cipher -= cipher[2];
    homo_cipher = homo_cipher - cipher[3];

    const Extended homo_dec = homo_cipher.decrypt(b);

    EXPECT_EQ(res_g, homo_dec);
}

TEST(ElGamal, HomomorphicMul) {
    auto [a, _, b, b_g] = generate();

    OsRng rng{};
    const std::array<Fr, 4> m = {Fr::random(rng), Fr::random(rng), Fr::random(rng), Fr::random(rng)};
    const std::array<Extended, 4> m_g = {
            GENERATOR_EXTENDED * m[0], GENERATOR_EXTENDED * m[1],
            GENERATOR_EXTENDED * m[2], GENERATOR_EXTENDED * m[3]
    };

    const Fr res = m[0] * m[1] * m[2] * m[3];
    const Extended res_g = GENERATOR_EXTENDED * res;

    const std::array<Cipher, 4> cipher = {
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[0]),
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[1]),
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[2]),
            Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g[3]),
    };
    Cipher homo_cipher = cipher[0] * m[1];
    homo_cipher *= m[2];
    homo_cipher = homo_cipher * m[3];

    const Extended homo_dec = homo_cipher.decrypt(b);

    EXPECT_EQ(res_g, homo_dec);
}

TEST(ElGamal, Serialize) {
    auto [a, _, b, b_g] = generate();

    OsRng rng{};
    const Fr m = Fr::random(rng);
    const Extended m_g = GENERATOR_EXTENDED * m;

    const Cipher cipher = Cipher::encrypt(a, b_g, GENERATOR_EXTENDED, m_g);
    const auto cipher_bytes = cipher.to_bytes();
    const auto cipher_recover = Cipher::from_bytes(cipher_bytes).value();

    const Extended decrypt = cipher_recover.decrypt(b);
    EXPECT_EQ(m_g, decrypt);
}