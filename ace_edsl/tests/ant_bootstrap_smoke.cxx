//-*-c++-*-
//=============================================================================
// Smoke test: call ANT rtlib's built-in Bootstrap() directly (no generated
// bootstrap_full kernel). Used by test_ant_bootstrap_smoke to verify the
// rtlib's Bootstrap runs and returns valid output. Does NOT compare to the
// sin(8x) Python reference (that is a different algorithm).
//=============================================================================

#include <cstring>
#include "rt_ant/rt_ant.h"
#include "ckks/cipher.h"

extern "C" void Run_ant_bootstrap_smoke(void) {
  CIPHERTEXT ciph = Get_input_data("p0", 0);
  CIPHERTEXT res;
  std::memset(&res, 0, sizeof(res));
  Bootstrap(&res, &ciph, 0);
  Set_output_data("output", 0, &res);
}
