#include <gtest/gtest.h>

#include "impl/os_rng.h"

#include "field/constant.h"
#include "group/affine.h"
#include "group/extended.h"
#include "group/constant.h"

using rng::impl::OsRng;

using jubjub::field::Fr;
using jubjub::field::constant::MODULUS;
using jubjub::field::constant::INV;
using jubjub::field::constant::R1;
using jubjub::field::constant::R2;
using jubjub::field::constant::R3;

using jubjub::group::Affine;
using jubjub::group::Extended;

using jubjub::group::constant::GENERATOR;

TEST(Fr, Inv) {
    uint64_t inv = 1;
    for (int i = 0; i < 63; ++i) {
        inv *= inv;
        inv *= 0xd0970e5ed6f72cb7;
    }
    inv = -inv;
    EXPECT_EQ(inv, INV);
}

TEST(Fr, Equality) {
    EXPECT_EQ(Fr::zero(), Fr::zero());
    EXPECT_EQ(Fr::one(), Fr::one());
    EXPECT_EQ(R2, R2);

    EXPECT_NE(Fr::zero(), Fr::one());
    EXPECT_NE(Fr::one(), R2);
}

TEST(Fr, ToBytes) {
    const std::array<uint8_t, 32> zero_bytes = Fr::zero().to_bytes();
    const std::array<uint8_t, 32> zero_bytes_expected = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0
    };
    EXPECT_EQ(zero_bytes, zero_bytes_expected);

    const std::array<uint8_t, 32> one_bytes = Fr::one().to_bytes();
    const std::array<uint8_t, 32> one_bytes_expected = {
            1, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0
    };
    EXPECT_EQ(one_bytes, one_bytes_expected);

    const std::array<uint8_t, 32> r2_bytes = R2.to_bytes();
    const std::array<uint8_t, 32> r2_bytes_expected = {
            217, 7, 150, 185, 179, 11, 248, 37,
            80, 231, 182, 102, 47, 214, 21, 243,
            244, 20, 136, 235, 238, 20, 37, 147,
            198, 85, 145, 71, 111, 252, 166, 9
    };
    EXPECT_EQ(r2_bytes, r2_bytes_expected);

    const std::array<uint8_t, 32> min_one_bytes = (-Fr::one()).to_bytes();
    const std::array<uint8_t, 32> min_one_bytes_expected = {
            182, 44, 247, 214, 94, 14, 151, 208,
            130, 16, 200, 204, 147, 32, 104, 166,
            0, 59, 52, 1, 1, 59, 103, 6,
            169, 175, 51, 101, 234, 180, 125, 14
    };
    EXPECT_EQ(min_one_bytes, min_one_bytes_expected);
}

TEST(Fr, FromBytes) {
    const Fr zero = Fr::from_bytes(
            {
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
            }
    ).value();
    EXPECT_EQ(zero, Fr::zero());

    const Fr one = Fr::from_bytes(
            {
                    1, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
            }
    ).value();
    EXPECT_EQ(one, Fr::one());

    const Fr r2 = Fr::from_bytes(
            {
                    217, 7, 150, 185, 179, 11, 248, 37,
                    80, 231, 182, 102, 47, 214, 21, 243,
                    244, 20, 136, 235, 238, 20, 37, 147,
                    198, 85, 145, 71, 111, 252, 166, 9
            }
    ).value();
    EXPECT_EQ(r2, R2);

    const auto min_one = Fr::from_bytes(
            {
                    182, 44, 247, 214, 94, 14, 151, 208,
                    130, 16, 200, 204, 147, 32, 104, 166,
                    0, 59, 52, 1, 1, 59, 103, 6,
                    169, 175, 51, 101, 234, 180, 125, 14
            }
    );
    EXPECT_TRUE(min_one.has_value());

    const auto modulus = Fr::from_bytes(
            {
                    183, 44, 247, 214, 94, 14, 151, 208,
                    130, 16, 200, 204, 147, 32, 104, 166,
                    0, 59, 52, 1, 1, 59, 103, 6,
                    169, 175, 51, 101, 234, 180, 125, 14
            }
    );
    EXPECT_FALSE(modulus.has_value());

    const auto modulus_l1 = Fr::from_bytes(
            {
                    184, 44, 247, 214, 94, 14, 151, 208,
                    130, 16, 200, 204, 147, 32, 104, 166,
                    0, 59, 52, 1, 1, 59, 103, 6,
                    169, 175, 51, 101, 234, 180, 125, 14
            }
    );
    const auto modulus_l2 = Fr::from_bytes(
            {
                    183, 44, 247, 214, 94, 14, 151, 208,
                    130, 16, 200, 204, 147, 32, 104, 166,
                    0, 59, 52, 1, 1, 59, 104, 6,
                    169, 175, 51, 101, 234, 180, 125, 14
            }
    );
    const auto modulus_l3 = Fr::from_bytes(
            {
                    183, 44, 247, 214, 94, 14, 151, 208,
                    130, 16, 200, 204, 147, 32, 104, 166,
                    0, 59, 52, 1, 1, 59, 103, 6,
                    169, 175, 51, 101, 234, 180, 125, 15
            }
    );
    EXPECT_FALSE(modulus_l1.has_value());
    EXPECT_FALSE(modulus_l2.has_value());
    EXPECT_FALSE(modulus_l3.has_value());
}

//TEST(Fr, Reduce) {
//    EXPECT_EQ(Fr::reduce(
//            {0xd0970e5ed6f72cb7, 0xa6682093ccc81082, 0x06673b0101343b00, 0x0e7db4ea6533afa9, 0, 0, 0, 0}), Fr::zero());
//
//}
//
//TEST(Fr, Reduce2) {
//    EXPECT_EQ(Fr::reduce({1, 0, 0, 0, 0, 0, 0, 0}), R1);
//}
//
//TEST(Fr, Reduce3) {
//    EXPECT_EQ(Fr::reduce({0, 0, 0, 0, 1, 0, 0, 0}), R2);
//}
//
//TEST(Fr, Reduce4) {
//    const uint64_t max = 0xffffffffffffffff;
//    EXPECT_EQ(Fr::reduce({max, max, max, max, max, max, max, max,}), R3 - R1);
//}

TEST(Fr, FromWide) {
    const Fr r2 = Fr::from_bytes_wide(
            {
                    217, 7, 150, 185, 179, 11, 248, 37, 80, 231, 182, 102, 47, 214, 21,
                    243, 244, 20, 136, 235, 238, 20, 37, 147, 198, 85, 145, 71, 111,
                    252, 166, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            }
    );

    const Fr neg_one = Fr::from_bytes_wide(
            {
                    182, 44, 247, 214, 94, 14, 151, 208, 130, 16, 200, 204, 147, 32,
                    104, 166, 0, 59, 52, 1, 1, 59, 103, 6, 169, 175, 51, 101, 234, 180,
                    125, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            }
    );

    const Fr maximum = Fr::from_bytes_wide(
            {
                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            }
    );
    const Fr maximum_expected{{0x8b75c9015ae42a22, 0xe59082e7bf9e38b8, 0x6440c91261da51b3, 0xa5e07ffb20991cf}};

    EXPECT_EQ(r2, R2);
    EXPECT_EQ(neg_one, -Fr::one());
    EXPECT_EQ(maximum, maximum_expected);
}

TEST(Fr, FromRaw) {
    Fr a = Fr::from_raw({0x25f80bb3b99607d8, 0xf315d62f66b6e750, 0x932514eeeb8814f4, 0x9a6fc6f479155c6});
    Fr b = Fr::from_raw({0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff});
    EXPECT_EQ(a, b);

    Fr c = Fr::from_raw({0xd0970e5ed6f72cb7, 0xa6682093ccc81082, 0x06673b0101343b00, 0x0e7db4ea6533afa9});
    EXPECT_EQ(c, Fr::zero());

    Fr d = Fr::from_raw({1, 0, 0, 0});
    EXPECT_EQ(d, R1);
}

TEST(Fr, Zero) {
    EXPECT_EQ(Fr::zero(), -Fr::zero());
    EXPECT_EQ(Fr::zero(), Fr::zero() + Fr::zero());
    EXPECT_EQ(Fr::zero(), Fr::zero() - Fr::zero());
    EXPECT_EQ(Fr::zero(), Fr::zero() * Fr::zero());
}

const Fr LARGEST{{0xd0970e5ed6f72cb6, 0xa6682093ccc81082, 0x06673b0101343b00, 0x0e7db4ea6533afa9}};

TEST(Fr, Add) {
    Fr temp = LARGEST;
    temp += LARGEST;
    Fr tt{{0xd0970e5ed6f72cb5, 0xa6682093ccc81082, 0x06673b0101343b00, 0x0e7db4ea6533afa9}};
    EXPECT_EQ(temp, tt);

    temp = LARGEST;
    temp += Fr{{1, 0, 0, 0}};
    EXPECT_EQ(temp, Fr::zero());
}

TEST(Fr, Neg) {
    Fr temp = -LARGEST;
    Fr tt{{1, 0, 0, 0}};
    EXPECT_EQ(temp, tt);

    temp = -Fr::zero();
    EXPECT_EQ(temp, Fr::zero());
    temp = -Fr{{1, 0, 0, 0}};
    EXPECT_EQ(temp, LARGEST);
}

TEST(Fr, Sub) {
    Fr temp = LARGEST;
    temp -= LARGEST;
    EXPECT_EQ(temp, Fr::zero());

    temp = Fr::zero();
    temp -= LARGEST;

    Fr temp2 = MODULUS;
    temp2 -= LARGEST;
    EXPECT_EQ(temp, temp2);
}

TEST(Fr, Mul) {
    Fr current = LARGEST;
    for (int i = 0; i < 100; ++i) {
        Fr temp = current;
        temp *= current;

        Fr temp2 = Fr::zero();
        auto bys = current.to_bytes();
        for (auto iter = bys.rbegin(); iter != bys.rend(); iter++) { // NOLINT(modernize-loop-convert)
            for (int j = 7; j >= 0; --j) {
                Fr temp3 = temp2;
                temp2 += temp3;
                if (((*iter >> j) & 0x01) == 0x01)
                    temp2 += current;
            }
        }
        EXPECT_EQ(temp2, temp);
        current += LARGEST;
    }
}

TEST(Fr, Squaring) {
    Fr current = LARGEST;
    for (int i = 0; i < 100; ++i) {
        Fr temp = current;
        temp = temp.square();

        Fr temp2 = Fr::zero();
        auto bys = current.to_bytes();
        for (auto iter = bys.rbegin(); iter != bys.rend(); iter++) { // NOLINT(modernize-loop-convert)
            for (int j = 7; j >= 0; --j) {
                Fr temp3 = temp2;
                temp2 += temp3;
                if (((*iter >> j) & 0x01) == 0x01)
                    temp2 += current;
            }
        }
        EXPECT_EQ(temp2, temp);
        current += LARGEST;
    }
}

TEST(Fr, Inversion) {
    EXPECT_FALSE(Fr::zero().invert().has_value());
    EXPECT_EQ(Fr::one().invert().value(), Fr::one());
    EXPECT_EQ((-Fr::one()).invert().value(), -Fr::one());

    Fr temp = R2;
    for (int i = 0; i < 100; ++i) {
        Fr temp2 = temp.invert().value();
        temp2 *= temp;
        EXPECT_EQ(temp2, Fr::one());
        temp += R2;
    }
}

TEST(Fr, InversionIsPow) {
    const std::array<uint64_t, 4> r_min_2{
            0xd0970e5ed6f72cb5, 0xa6682093ccc81082,
            0x06673b0101343b00, 0x0e7db4ea6533afa9,
    };
    Fr r1 = R1;
    Fr r2 = R1;

    for (int i = 0; i < 100; ++i) {
        r1 = r1.invert().value();
        r2 = r2.pow(r_min_2);

        EXPECT_EQ(r1, r2);

        r1 += R1;
        r2 = r1;
    }
}

TEST(Fr, Sqrt) {
    Fr square{{0xd0970e5ed6f72cb5, 0xa6682093ccc81082, 0x06673b0101343b00, 0x0e7db4ea6533afa9}};
    int32_t none_count = 0;

    for (int i = 0; i < 100; ++i) {
        const auto square_root = square.sqrt();
        if (!square_root.has_value()) {
            none_count++;
        } else {
            EXPECT_EQ(square_root.value() * square_root.value(), square);
        }
        square -= Fr::one();
    }
    EXPECT_EQ(47, none_count);
}

TEST(Fr, NAF) {
    const Fr fr{1122334455ULL};
    const std::array<int8_t, 31> naf3_fr = {
            -1, 0, 0, -1, 0, 0, 0, 0,
            -1, 0, 0, -1, 0, 0, 0, 3,
            0, 0, 1, 0, 0, -1, 0, 0,
            3, 0, 0, 0, 0, 0, 1,
    };
    const auto computed = fr.compute_windowed_non_adjacent(3);
    std::array<int8_t, 31> buf{};
    std::copy(computed.begin(), computed.begin() + 31, buf.begin());

    EXPECT_EQ(naf3_fr, buf);
}

TEST(Fr, FromBytes2) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        EXPECT_EQ(a, Fr::from_bytes(a.to_bytes()).value());
    }
}

TEST(Fr, AddAssociativity) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        Fr b = Fr::random(rng);
        Fr c = Fr::random(rng);
        EXPECT_EQ((a + b) + c, a + (b + c));
    }
}

TEST(Fr, AddInv) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        Fr a_neg = -a;
        EXPECT_EQ(Fr::zero(), a + a_neg);
        EXPECT_EQ(Fr::zero(), a_neg + a);
    }
}

TEST(Fr, AddCommutativity) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        Fr b = Fr::random(rng);
        EXPECT_EQ(a + b, b + a);
    }
}

TEST(Fr, AddIdentity) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        EXPECT_EQ(a, a + Fr::zero());
        EXPECT_EQ(a, Fr::zero() + a);
    }
}

TEST(Fr, SubIdentity) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        EXPECT_EQ(a, a - Fr::zero());
        EXPECT_EQ(a, Fr::zero() - (-a));
    }
}

TEST(Fr, MulAssociativity) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        Fr b = Fr::random(rng);
        Fr c = Fr::random(rng);
        EXPECT_EQ((a * b) * c, a * (b * c));
    }
}

TEST(Fr, MulIdentity) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        EXPECT_EQ(a, a * Fr::one());
        EXPECT_EQ(a, Fr::one() * a);
    }
}

TEST(Fr, MulInv) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        if (a == Fr::zero()) continue;

        Fr a_inv = a.invert().value();
        EXPECT_EQ(Fr::one(), a * a_inv);
        EXPECT_EQ(Fr::one(), a_inv * a);
    }
}

TEST(Fr, MulCommutativity) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        Fr b = Fr::random(rng);
        EXPECT_EQ(a * b, b * a);
    }
}

TEST(Fr, MulAddIdentity) {
    OsRng rng{};
    for (int i = 0; i < 1000; ++i) {
        Fr a = Fr::random(rng);
        EXPECT_EQ(Fr::zero(), Fr::zero() * a);
        EXPECT_EQ(Fr::zero(), a * Fr::zero());
    }
}

Affine dhke(const Fr &secret, const Extended &pub) {
    return Affine{pub * secret};
}

TEST(Fr, DHKE) {
    OsRng rng{};
    const Extended g = Extended{GENERATOR};
    for (int i = 0; i < 1000; ++i) {
        const Fr a = Fr::random(rng);
        const Fr b = Fr::random(rng);

        const auto a_g = g * a;
        const auto b_g = g * b;

        EXPECT_EQ(dhke(a, b_g), dhke(b, a_g));
        EXPECT_NE(dhke(a, b_g), dhke(b, b_g));
    }
}