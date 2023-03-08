#include <gtest/gtest.h>

#include <array>

#include "scalar/scalar.h"

#include "field/fr.h"
#include "group/affine.h"
#include "group/affine_niels.h"
#include "group/extended.h"
#include "group/extended_niels.h"
#include "group/constants.h"
#include "group/normalize.h"

using bls12_381::scalar::Scalar;

using jubjub::field::Fr;

using jubjub::group::Affine;
using jubjub::group::AffineNiels;
using jubjub::group::Extended;
using jubjub::group::ExtendedNiels;

using jubjub::group::batch_normalize;

using jubjub::group::constant::GENERATOR;
using jubjub::group::constant::GENERATOR_EXTENDED;
using jubjub::group::constant::GENERATOR_NUMS;
using jubjub::group::constant::EDWARDS_D1;
using jubjub::group::constant::EDWARDS_D2;
using jubjub::group::constant::FR_MODULUS_BYTES;

TEST(Group, OnCurve) {
    EXPECT_TRUE(Affine::identity().is_on_curve());
}

TEST(Group, AffineGenerator) {
    EXPECT_TRUE(GENERATOR.is_prime_order());
    EXPECT_FALSE(Extended{GENERATOR.mul_by_cofactor()}.is_identity());

    EXPECT_TRUE(GENERATOR_NUMS.is_prime_order());
    EXPECT_FALSE(Extended{GENERATOR_NUMS.mul_by_cofactor()}.is_identity());

}

TEST(Group, ExtendedGenerator) {
    EXPECT_TRUE(GENERATOR_EXTENDED.is_prime_order());
    EXPECT_FALSE(Extended{GENERATOR_EXTENDED.mul_by_cofactor()}.is_identity());
}

TEST(Group, EdwardsD1) {
    EXPECT_FALSE(EDWARDS_D1.sqrt().has_value());
    EXPECT_FALSE((-EDWARDS_D1).sqrt().has_value());
    EXPECT_FALSE((-EDWARDS_D1).invert().value().sqrt().has_value());
}

TEST(Group, AffineNiels) {
    EXPECT_EQ(AffineNiels::identity().get_y_plus_x(), AffineNiels{Affine::identity()}.get_y_plus_x());
    EXPECT_EQ(AffineNiels::identity().get_y_minus_x(), AffineNiels{Affine::identity()}.get_y_minus_x());
    EXPECT_EQ(AffineNiels::identity().get_t2d(), AffineNiels{Affine::identity()}.get_t2d());
}

TEST(Group, ExtendedNiels) {
    EXPECT_EQ(ExtendedNiels::identity().get_y_plus_x(), ExtendedNiels{Extended::identity()}.get_y_plus_x());
    EXPECT_EQ(ExtendedNiels::identity().get_y_minus_x(), ExtendedNiels{Extended::identity()}.get_y_minus_x());
    EXPECT_EQ(ExtendedNiels::identity().get_z(), ExtendedNiels{Extended::identity()}.get_z());
    EXPECT_EQ(ExtendedNiels::identity().get_t2d(), ExtendedNiels{Extended::identity()}.get_t2d());
}

TEST(Group, Assoc) {
    const Extended p = Extended{Affine{
            Scalar::from_raw({0x81c571e5d883cfb0, 0x049f7a686f147029, 0xf539c860bc3ea21f, 0x4284715b7ccc8162}),
            Scalar::from_raw({0xbf096275684bb8ca, 0xc7ba245890af256d, 0x59119f3e86380eb0, 0x3793de182f9fb1d2}),
    }}.mul_by_cofactor();

    EXPECT_TRUE(p.is_on_curve());
    EXPECT_EQ((p * Fr{1000ULL}) * Fr{3938ULL}, p * (Fr{1000ULL} * Fr{3938ULL}));
}

TEST(Group, BatchNormalize) {
    Extended p = Extended{Affine{
            Scalar::from_raw({0x81c571e5d883cfb0, 0x049f7a686f147029, 0xf539c860bc3ea21f, 0x4284715b7ccc8162}),
            Scalar::from_raw({0xbf096275684bb8ca, 0xc7ba245890af256d, 0x59119f3e86380eb0, 0x3793de182f9fb1d2}),
    }}.mul_by_cofactor();

    std::vector<Extended> y{};
    y.reserve(10);
    for (int i = 0; i < 10; ++i) {
        y.push_back(p);
        p = p.doubles();
    }

    for (const Extended &point: y)
        EXPECT_TRUE(point.is_on_curve());

    std::vector<Affine> expected{};
    expected.reserve(y.size());
    for (const Extended &point: y) expected.emplace_back(point);

    const std::vector<Affine> result1 = batch_normalize(y);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(expected[i], result1[i]);
        EXPECT_TRUE(y[i].is_on_curve());
        EXPECT_EQ(Affine{y[i]}, expected[i]);
    }

    const std::vector<Affine> result2 = batch_normalize(y);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(expected[i], result2[i]);
        EXPECT_TRUE(y[i].is_on_curve());
        EXPECT_EQ(Affine{y[i]}, expected[i]);
    }
}


Affine FULL_GENERATOR = Affine{
        Scalar{{0x50c87a58c166eca5, 0x8046fd74c0051afc, 0x406355ee695b0493, 0x0d5a8d931bdc7e0a}},
        Scalar{{0x00000017ffffffe8, 0x26389fb800276018, 0x3293bf3f18d3bf80, 0x21b85034193c413b}}
};

std::array<Affine, 8> EIGHT_TORSION = {
        Affine{
                Scalar{{0xad54905676840a17, 0x884a28f1a8cee9b4, 0xdfcc6227f79d2e0c, 0x45784f13df4a06a9}},
                Scalar{{0xdc2e8792ad17413b, 0x22a13f6d0d805e26, 0x87876d4df48e7492, 0x3e344d8cbceee813}},
        },
        Affine{
                Scalar{{0x0c4fa98a55763050, 0x4c8ea2c29ff7a200, 0x649fca48e43b5ddf, 0x26c0c34dfc43f9d3}},
                Scalar{{0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}},
        },
        Affine{
                Scalar{{0xad54905676840a17, 0x884a28f1a8cee9b4, 0xdfcc6227f79d2e0c, 0x45784f13df4a06a9}},
                Scalar{{0x23d1786c52e8bec6, 0x311c6495f27dfdd8, 0xabb26aba15136373, 0x35b959c66cae9534}},
        },
        Affine{
                Scalar{{0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}},
                Scalar{{0xfffffffd00000003, 0xfb38ec08fffb13fc, 0x99ad88181ce5880f, 0x5bc8f5f97cd877d8}},
        },
        Affine{
                Scalar{{0x52ab6fa8897bf5ea, 0xcb737b11572f724a, 0x536d75e01204a9f8, 0x2e75583f4a53769e}},
                Scalar{{0x23d1786c52e8bec6, 0x311c6495f27dfdd8, 0xabb26aba15136373, 0x35b959c66cae9534}},
        },
        Affine{
                Scalar{{0xf3b05674aa89cfb1, 0x072f01406006b9fe, 0xce9a0dbf25667a26, 0x4d2ce4052d598374}},
                Scalar{{0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}},
        },
        Affine{
                Scalar{{0x52ab6fa8897bf5ea, 0xcb737b11572f724a, 0x536d75e01204a9f8, 0x2e75583f4a53769e}},
                Scalar{{0xdc2e8792ad17413b, 0x22a13f6d0d805e26, 0x87876d4df48e7492, 0x3e344d8cbceee813}},
        },
        Affine{
                Scalar{{0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}},
                Scalar{{0x00000001fffffffe, 0x5884b7fa00034802, 0x998c4fefecbc4ff5, 0x1824b159acc5056f}},
        },
};

TEST(Group, EightTorsion) {
    const Extended g1{FULL_GENERATOR};
    const Extended g2 = g1.multiply(FR_MODULUS_BYTES);

    EXPECT_FALSE(g1.is_small_order());
    EXPECT_TRUE(g2.is_small_order());

    Extended cur = g2;

    for (const auto &affine: EIGHT_TORSION) {
        const Affine temp = Affine{cur};
        EXPECT_EQ(temp, affine);
        cur += g2;
    }
}

TEST(Group, CurveGen) {
    std::array<uint8_t, 32> trial_bytes{};
    for (int i = 0; i < 255; ++i) {
        const auto a_opt = Affine::from_bytes(trial_bytes);
        if (a_opt.has_value()) {
            const Affine &a = a_opt.value();
            EXPECT_TRUE(a.is_on_curve());

            Extended b = Extended{a};
            b = b.multiply(FR_MODULUS_BYTES);
            EXPECT_TRUE(b.is_small_order());

            b = b.doubles();
            EXPECT_TRUE(b.is_small_order());

            b = b.doubles();
            EXPECT_TRUE(b.is_small_order());

            if (!b.is_identity()) {
                b = b.doubles();
                EXPECT_TRUE(b.is_small_order());
                EXPECT_TRUE(b.is_identity());
                EXPECT_EQ(FULL_GENERATOR, a);
                EXPECT_TRUE(a.mul_by_cofactor().is_torsion_free());
                return;
            }
        }
        trial_bytes[0] += 1;
    }
    EXPECT_TRUE(false);
}

TEST(Group, SmallOrder) {
    for (const auto &affine: EIGHT_TORSION) {
        EXPECT_TRUE(affine.is_small_order());
    }
}

TEST(Group, IsIdentity) {
    const Extended a = EIGHT_TORSION[0].mul_by_cofactor();
    const Extended b = EIGHT_TORSION[1].mul_by_cofactor();

    EXPECT_EQ(a.get_x(), b.get_x());
    EXPECT_EQ(a.get_y(), a.get_z());
    EXPECT_EQ(b.get_y(), b.get_z());
    EXPECT_NE(a.get_y(), b.get_y());
    EXPECT_NE(a.get_z(), b.get_z());

    EXPECT_TRUE(a.is_identity());
    EXPECT_TRUE(b.is_identity());

    for (const auto &affine: EIGHT_TORSION) {
        EXPECT_TRUE(affine.mul_by_cofactor().is_identity());
    }
}

TEST(Group, MulConsistency) {
    const Fr a{{0x21e61211d9934f2e, 0xa52c058a693c3e07, 0x9ccb77bfb12d6360, 0x07df2470ec94398e}};
    const Fr b{{0x03336d1cbe19dbe0, 0x0153618f6156a536, 0x2604c9e1fc3c6b15, 0x04ae581ceb028720}};
    const Fr c{{0xd7abf5bb24683f4c, 0x9d7712cc274b7c03, 0x973293db9683789f, 0x0b677e29380a97a7}};

    EXPECT_EQ(a * b, c);

    const Extended p = Extended{Affine{
            Scalar::from_raw({0x81c571e5d883cfb0, 0x049f7a686f147029, 0xf539c860bc3ea21f, 0x4284715b7ccc8162}),
            Scalar::from_raw({0xbf096275684bb8ca, 0xc7ba245890af256d, 0x59119f3e86380eb0, 0x3793de182f9fb1d2}),
    }}.mul_by_cofactor();
    EXPECT_EQ(p * c, (p * a) * b);

    // Test Mul implemented on ExtendedNielsPoint
    EXPECT_EQ(p * c, (ExtendedNiels{p} * a) * b);
    EXPECT_EQ(ExtendedNiels{p} * c, (p * a) * b);
    EXPECT_EQ(ExtendedNiels{p} * c, (ExtendedNiels{p} * a) * b);

    // Test Mul implemented on AffineNielsPoint
    const AffineNiels p_affine_niels = AffineNiels{Affine{p}};
    EXPECT_EQ(p * c, (p_affine_niels * a) * b);
    EXPECT_EQ(p_affine_niels * c, (p * a) * b);
    EXPECT_EQ(p_affine_niels * c, (p_affine_niels * a) * b);
}

TEST(Group, SerializationConsistency) {
    const Extended gen = FULL_GENERATOR.mul_by_cofactor();
    Extended p = gen;

    std::vector<std::array<uint8_t, 32>> y = {
            {
                    203, 85,  12,  213, 56,  234, 12,  193, 19,  132, 128, 64,
                                                                                142, 110, 170, 185, 179, 108, 97,  63,  13,  211,
                                                                                                                                  247, 120, 79,  219, 110, 234, 131, 123, 19,  215,},
            {
                    113, 154, 240, 230, 224, 198, 208, 170, 104, 15,  59,
                                                                           126, 151, 222, 233, 195, 203, 195, 167, 129, 89,  121,
                                                                                                                                  240, 142, 51,  166, 64,  250, 184, 202, 154, 177,
            },
            {
                    197, 41,  93,  209, 203, 55,  164, 174, 88,  0,   90,
                                                                           199, 1,   156, 149, 141, 240, 29,  14,  82,  86,  225,
                                                                                                                                  126, 129, 186, 157, 148, 162, 219, 51,  156, 199,
            },
            {
                    182, 117, 250, 241, 81,  196, 199, 227, 151, 74,
                                                                      243, 17,  221, 97,  200, 139, 192, 83,  231, 35,
                                                                                                                        214, 14,  95,  69,  130, 201, 4,   116, 177, 19,  179, 0,
            },
            {
                    118, 41,  29,  200, 60,  189, 119, 252, 78,  40,
                                                                      230, 18,  208, 221, 38,  214, 176, 250, 4,   10,  77,
                                                                                                                             101, 26,  216, 193, 198, 226, 84,  25,  177, 230, 185,
            },
            {
                    226, 189, 227, 208, 112, 117, 136, 98,  72,  38,
                                                                      211, 167, 254, 82,  174, 113, 112, 166, 138, 171,
                                                                                                                        166, 113, 52,  251, 129, 197, 138, 45,  195, 7,   61,  140,
            },
            {
                    38,  198, 156, 196, 146, 225, 55,  163, 138, 178,
                                                                      157, 128, 115, 135, 204, 215, 0,   33,  171, 20,
                                                                                                                        60,  32,  142, 209, 33,  233, 125, 146, 207, 12,  16,  24,
            },
            {
                    17,  187, 231, 83,  165, 36,  232, 184, 140, 205,
                                                                      195, 252, 166, 85,  59,  86,  3,   226, 211, 67,
                                                                                                                        179, 29,  238, 181, 102, 142, 58,  63,  57,  89,  174, 138,
            },
            {
                    210, 159, 80,  16,  181, 39,  221, 204, 224,
                                                                 144, 145, 79,  54,  231, 8,   140, 142, 216, 93,
                                                                                                                   190, 183, 116, 174, 63,  33,  242, 177, 118, 148, 40,  241, 203,
            },
            {
                    0,   143, 107, 102, 149, 187, 27,  124, 18,  10,
                                                                      98,  28,  113, 123, 121, 185, 29,  152, 14,  130, 149,
                                                                                                                             28,  87,  35,  135, 135, 153, 54,  112, 53,  54,  68,
            },
            {
                    178, 131, 85,  160, 214, 51,  208, 157, 196, 152,
                                                                      247, 93,  202, 56,  81,  239, 155, 122, 59,  188, 237,
                                                                                                                             253, 11,  169, 208, 236, 12,  4,   163, 211, 88,  97,
            },
            {
                    246, 194, 231, 195, 159, 101, 180, 133, 80,  21,
                                                                      185, 220, 195, 115, 144, 12,  90,  150, 44,  117, 8,
                                                                                                                             156, 168, 248, 206, 41,  60,  82,  67,  75,  57,  67,
            },
            {
                    212, 205, 171, 153, 113, 16,  194, 241, 224, 43,
                                                                      177, 110, 190, 248, 22,  201, 208, 166, 2,   83,
                                                                                                                        134, 130, 85,  129, 166, 136, 185, 191, 163, 38,  54,  10,
            },
            {
                    8,   60,  190, 39,  153, 222, 119, 23,  142, 237,
                                                                      12,  110, 146, 9,   19,  219, 143, 64,  161, 99,
                                                                                                                        199, 77,  39,  148, 70,  213, 246, 227, 150, 178, 237, 178,
            },
            {
                    11,  114, 217, 160, 101, 37,  100, 220, 56,  114,
                                                                      42,  31,  138, 33,  84,  157, 214, 167, 73,  233,
                                                                                                                        115, 81,  124, 134, 15,  31,  181, 60,  184, 130, 175, 159,
            },
            {
                    141, 238, 235, 202, 241, 32,  210, 10,  127, 230,
                                                                      54,  31,  146, 80,  247, 9,   107, 124, 0,   26,
                                                                                                                        203, 16,  237, 34,  214, 147, 133, 15,  29,  236, 37,  88,
            },
    };
    for (const auto &expected: y) {
        EXPECT_TRUE(p.is_on_curve());
        const Affine affine = Affine{p};
        const auto serialized = affine.to_bytes();
        const auto deserialized = Affine::from_bytes(serialized).value();
        EXPECT_EQ(affine, deserialized);
        EXPECT_EQ(expected, serialized);
        p += gen;
    }
}

TEST(Group, Zip216) {
    const std::vector<std::array<uint8_t, 32>> NON_CANONICAL_ENCODINGS = {
            {
                    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
            },
            {
                    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
                    0xfe, 0x5b, 0xfe, 0xff, 0x02, 0xa4, 0xbd, 0x53,
                    0x05, 0xd8, 0xa1, 0x09, 0x08, 0xd8, 0x39, 0x33,
                    0x48, 0x7d, 0x9d, 0x29, 0x53, 0xa7, 0xed, 0xf3,
            }
    };

    for (const auto &b: NON_CANONICAL_ENCODINGS) {
        std::array<uint8_t, 32> encoding = b;
        EXPECT_FALSE(Affine::from_bytes(encoding).has_value());
        encoding[31] &= 0b01111111;
        EXPECT_TRUE(Affine::from_bytes(encoding).has_value());
    }
}