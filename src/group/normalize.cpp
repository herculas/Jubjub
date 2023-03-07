#include "group/normalize.h"

namespace jubjub::group {

using bls12_381::scalar::Scalar;

auto batch_normalize(std::vector<Extended> &y) -> std::vector<Affine> {
    Scalar acc = Scalar::one();
    for (Extended &p: y) {
        p.set_t1(acc);
        acc *= p.get_z();
    }

    acc = acc.invert().value();

    for (auto iter = y.rbegin(); iter != y.rend(); iter++) { // NOLINT(modernize-loop-convert)
        Extended q = *iter;
        const Scalar temp = q.get_t1() * acc;

        acc *= q.get_z();

        q.set_x(q.get_x() * temp);
        q.set_y(q.get_y() * temp);
        q.set_z(Scalar::one());
        q.set_t1(q.get_x());
        q.set_t2(q.get_y());

        *iter = q;
    }

    std::vector<Affine> res;
    res.reserve(y.size());
    for (const Extended &point: y)
        res.emplace_back(point.get_x(), point.get_y());

    return res;
}

} // namespace jubjub::group