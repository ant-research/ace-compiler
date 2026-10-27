#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "phantom.h"

using namespace std;
using namespace phantom;
int main() {
    EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = 8192;
    double scale = pow(2.0, 40);

    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(poly_modulus_degree, {60, 40, 40, 60}));
    PhantomContext context(parms);

    PhantomCKKSEncoder encoder(context);

    vector<double> coeffs = {1.25, -2.5, 3.75, -4.0, 5.5, 0.0, -7.25, 8.125};
    PhantomPlaintext plain;
    encoder.encode_coeffs(context, coeffs, scale, plain);

    vector<double> decoded;
    encoder.decode_coeffs(context, plain, decoded);

    double max_error = 0.0;
    for (size_t i = 0; i < coeffs.size(); i++) {
        max_error = max(max_error, fabs(decoded[i] - coeffs[i]));
    }

    cout << "Max error: " << max_error << endl;
    cout << "Decoded coefficients (first 8): ";
    for (size_t i = 0; i < coeffs.size(); i++) {
        cout << decoded[i] << (i + 1 == coeffs.size() ? "\n" : ", ");
    }
    return 0;
}
