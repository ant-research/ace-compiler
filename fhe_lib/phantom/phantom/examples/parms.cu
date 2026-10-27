
#include <random>
#include "phantom.h"

using namespace std;
using namespace phantom;

int main()
{
    size_t slots = 32;
    phantom::EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = slots << 1;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(poly_modulus_degree, {40, 40, 40, 40}));

    PhantomContext context(parms);
    PhantomSecretKey secret_key(context);
    PhantomPublicKey public_key = secret_key.gen_publickey(context);
    PhantomRelinKey relin_keys = secret_key.gen_relinkey(context);
    PhantomGaloisKey galois_keys;

    PhantomCKKSEncoder encoder(context);

    
    return 0;
}