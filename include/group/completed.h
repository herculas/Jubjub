#ifndef JUBJUB_COMPLETED_H
#define JUBJUB_COMPLETED_H

#include "scalar/scalar.h"

namespace jubjub::group {

struct Completed {
    bls12_381::scalar::Scalar x;
    bls12_381::scalar::Scalar y;
    bls12_381::scalar::Scalar z;
    bls12_381::scalar::Scalar t;
};

} // namespace jubjub::group

#endif //JUBJUB_COMPLETED_H