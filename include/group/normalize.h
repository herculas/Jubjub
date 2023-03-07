#ifndef JUBJUB_NORMALIZE_H
#define JUBJUB_NORMALIZE_H

#include <vector>

#include "scalar/scalar.h"

#include "group/affine.h"
#include "group/extended.h"

namespace jubjub::group {

auto batch_normalize(std::vector<Extended> &y) -> std::vector<Affine>;

} // namespace jubjub::group

#endif //JUBJUB_NORMALIZE_H