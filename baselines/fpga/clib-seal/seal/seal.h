// SEAL-compatibility shim: maps FHE-MP-CNN's single include "seal/seal.h"
// onto the SEAL-compatible API exported by the clib SDK wrapper (seal_api.h,
// namespace seal). The clib SDK is an evaluation-only backend (opaque clib
// handles); this header only adds the few source-compatibility bits the CNN
// code expects on top of what seal_api.h already provides.
#ifndef FHE_MP_CNN_CLIB_SEAL_SHIM_H
#define FHE_MP_CNN_CLIB_SEAL_SHIM_H

// The whole SEAL 3.6 surface used by cnn_ckks lives in one shim header.
#include "seal_api.h"

// FHE-MP-CNN does `using namespace seal::util;` in several headers
// (SEALcomp.h, Polynomial.h, Bootstrapper.h). The namespace must exist for
// that to be well-formed even though no util symbol is actually referenced.
namespace seal
{
    namespace util
    {
        // Original FHE-MP-CNN headers declare `vector<RNSIter>` members that
        // are never used; the clib backend has no raw-RNS iterator. Provide a
        // complete placeholder type so those declarations still parse.
        struct RNSIter
        {
        };

        // `seal::util::iter(...)` is used by the CNN code only to range over
        // the coefficient modulus vector, i.e. `iter(parms.coeff_modulus())`.
        // Forwarding to the container preserves the operator[]/value() use.
        template <class Container>
        const Container &iter(const Container &container)
        {
            return container;
        }
    } // namespace util
} // namespace seal

// The FHE-MP-CNN SEAL fork adds KeyGenerator::create_hoisted_galois_keys.
// In the clib backend all rotation keys are supplied by the material
// directory, so the hoisted-key variant has no separate meaning: alias it to
// the plain galois-key creation that seal_api.h provides.
#ifndef create_hoisted_galois_keys
#define create_hoisted_galois_keys create_galois_keys
#endif

#endif // FHE_MP_CNN_CLIB_SEAL_SHIM_H
