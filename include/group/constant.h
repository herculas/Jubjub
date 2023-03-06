#ifndef JUBJUB_GROUP_CONSTANT_H
#define JUBJUB_GROUP_CONSTANT_H

#include <array>

#include "scalar/scalar.h"

#include "group/affine.h"
#include "group/extended.h"

namespace jubjub::group::constant {

const std::array<uint8_t, 32> FR_MODULUS_BYTES = {
        183, 44, 247, 214, 94, 14, 151, 208,
        130, 16, 200, 204, 147, 32, 104, 166,
        0, 59, 52, 1, 1, 59, 103, 6,
        169, 175, 51, 101, 234, 180, 125, 14,
};

const Affine GENERATOR{
        bls12_381::scalar::Scalar{
                {
                        0xc8cd898c547c71aa, 0x1e77bad0b3564650,
                        0x0b5183a649031ebe, 0x4f54a483a3031a2c,
                }
        },
        bls12_381::scalar::Scalar{
                {
                        0x00000026ffffffd9, 0x3e1c038b003ffc27,
                        0x323016c688581730, 0x56cb8254a901ea00,
                }
        },
};

const Affine GENERATOR_NUMS{
        bls12_381::scalar::Scalar{
                {
                        0x51d37e7271c3e812, 0xf3ad45392074aaa8,
                        0x21bb2537c0cfbca7, 0x0bb829228bf29c9c,
                }
        },
        bls12_381::scalar::Scalar{
                {
                        0x36fba2bf0c68cf00, 0xcd442b52d2b7f2ad,
                        0xbe025c79f9f895d4, 0x61e43e3f466dbd00,
                }
        },
};

const Extended GENERATOR_EXTENDED{
        bls12_381::scalar::Scalar{
                {
                        0xc8cd898c547c71aa, 0x1e77bad0b3564650,
                        0x0b5183a649031ebe, 0x4f54a483a3031a2c,
                }
        },
        bls12_381::scalar::Scalar{
                {
                        0x00000026ffffffd9, 0x3e1c038b003ffc27,
                        0x323016c688581730, 0x56cb8254a901ea00,
                }
        },
        bls12_381::scalar::Scalar{
                {
                        0x00000001fffffffe, 0x5884b7fa00034802,
                        0x998c4fefecbc4ff5, 0x1824b159acc5056f,
                }
        },
        bls12_381::scalar::Scalar{
                {
                        0xc8cd898c547c71aa, 0x1e77bad0b3564650,
                        0x0b5183a649031ebe, 0x4f54a483a3031a2c,
                }
        },
        bls12_381::scalar::Scalar{
                {
                        0x00000026ffffffd9, 0x3e1c038b003ffc27,
                        0x323016c688581730, 0x56cb8254a901ea00,
                }
        },
};

const Extended GENERATOR_NUMS_EXTENDED{
        bls12_381::scalar::Scalar{
                {
                        0x51d37e7271c3e812, 0xf3ad45392074aaa8,
                        0x21bb2537c0cfbca7, 0x0bb829228bf29c9c,
                }
        },
        bls12_381::scalar::Scalar{
                {
                        0x36fba2bf0c68cf00, 0xcd442b52d2b7f2ad,
                        0xbe025c79f9f895d4, 0x61e43e3f466dbd00,
                }
        },
        bls12_381::scalar::Scalar{
                {
                        0x00000001fffffffe, 0x5884b7fa00034802,
                        0x998c4fefecbc4ff5, 0x1824b159acc5056f,
                }
        },
        bls12_381::scalar::Scalar{
                {
                        0x51d37e7271c3e812, 0xf3ad45392074aaa8,
                        0x21bb2537c0cfbca7, 0x0bb829228bf29c9c,
                }
        },
        bls12_381::scalar::Scalar{
                {
                        0x36fba2bf0c68cf00, 0xcd442b52d2b7f2ad,
                        0xbe025c79f9f895d4, 0x61e43e3f466dbd00,
                }
        },
};

const bls12_381::scalar::Scalar EDWARDS_D1 = bls12_381::scalar::Scalar{
        {
                0x2a522455b974f6b0, 0xfc6cc9ef0d9acab3,
                0x7a08fb94c27628d1, 0x57f8f6a8fe0e262e,
        }
};

const bls12_381::scalar::Scalar EDWARDS_D2 = bls12_381::scalar::Scalar{
        {
                0x54a448ac72e9ed5f, 0xa51befdb1b373967,
                0xc0d81f217b4a799e, 0x3c0445fed27ecf14,
        }
};

} // namespace jubjub::group::constant

#endif //JUBJUB_GROUP_CONSTANT_H
