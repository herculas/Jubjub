#ifndef JUBJUB_FIELD_CONSTANT_H
#define JUBJUB_FIELD_CONSTANT_H

#include "field/fr.h"

namespace jubjub::field::constant {

const Fr MODULUS(
        {
                0xd0970e5ed6f72cb7, 0xa6682093ccc81082,
                0x06673b0101343b00, 0x0e7db4ea6533afa9,
        }
);

const uint64_t INV = 0x1ba3a358ef788ef9;

const Fr R1(
        {
                0x25f80bb3b99607d9, 0xf315d62f66b6e750,
                0x932514eeeb8814f4, 0x09a6fc6f479155c6,
        }
);

const Fr R2(
        {
                0x67719aa495e57731, 0x51b0cef09ce3fc26,
                0x69dab7fac026e9a5, 0x04f6547b8d127688,
        }
);

const Fr R3(
        {
                0xe0d6c6563d830544, 0x323e3883598d0f85,
                0xf0fea3004c2e2ba8, 0x05874f84946737ec,
        }
);

} // namespace jubjub::field::constant

#endif //JUBJUB_FIELD_CONSTANT_H