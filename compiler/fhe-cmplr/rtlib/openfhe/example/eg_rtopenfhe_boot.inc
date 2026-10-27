// external header files
#include "rt_openfhe/rt_openfhe.h"

#include <cmath>
#include <cstdint>
#include <vector>

typedef float float32_t;

// Bootstrap smoke: force the OPENFHE_CONTEXT constructor onto the
// EvalBootstrap branch.
bool Need_bts() { return true; }

// Io_init() (called by Prepare_context) reads these weak symbols; mirror the
// plain examples (single input "input", single output "output").
int Get_input_count() { return 1; }
DATA_SCHEME* Get_encode_scheme(int /*idx*/) {
  static MAP_DESC   desc_0[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme_0 = {"input", {0, 0, 0, 0}, 1, desc_0};
  static DATA_SCHEME* scheme[] = {&scheme_0};
  return scheme[0];
}
int Get_output_count() { return 1; }
DATA_SCHEME* Get_decode_scheme(int /*idx*/) {
  static MAP_DESC   desc[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme = {"output", {0, 0, 0, 0}, 1, desc};
  return &scheme;
}

// refreshed by Main_graph(); read by the .cxx main to OR the level-refresh
// verdict into the final PASS/FAIL.
int g_boot_level_ok = 0;

CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
      LIB_OPENFHE,  // _provider
      4096,         // _poly_degree   (ring dim; OpenFHE bootstrap minimum ring)
      0,            // _sec_level     -> HEStd_NotSet (avoid ring auto-raise)
      29,           // _mul_depth     (gives depth = 35 >> bts_depth=22: refresh headroom)
      0,            // _input_level
      60,           // _first_mod_size  (canonical simple-ckks-bootstrapping 64-bit branch: 60/59)
      59,           // _scaling_mod_size (FIXEDMANUAL; == FHE_BTS_SF default)
      0,            // _num_q_parts
      0,            // _hamming_weight  -> c_bts_d = 19 in the ctor sizing
      0,            // _num_rot_idx
      {}            // _rot_idxs
  };
  return &parm;
}

bool Main_graph() {
  CIPHERTEXT input = Get_input_data("input", 0);

  // depth = fresh cipher tower count - 1 == _num_primes - 1
  size_t fresh_towers = input->GetElements()[0].GetNumOfElements();
  size_t depth        = fresh_towers - 1;

  // deplete to GetLevel = depth - 2 (leave 2 levels). mul_by_plain of a
  // full-slots all-1.0 plaintext (so every slot is preserved, not just slot 0)
  // + rescale, scale-matched each step by re-encoding at the cipher's current
  // level.
  size_t            num_slots = Degree() / 2;
  std::vector<float> ones(num_slots, 1.0f);
  while (Level(&input) < depth - 2) {
    uint32_t  lvl = static_cast<uint32_t>(Level(&input));
    PLAINTEXT pt_one;
    Encode_float(&pt_one, ones.data(), num_slots, /*sc_degree=*/1,
                 /*level=*/lvl);
    CIPHERTEXT tmp;
    Mul_plain(&tmp, &input, &pt_one);
    Rescale_ciph(&input, &tmp);
  }

  uint64_t pre_level = Level(&input);
  size_t   pre_avail = depth - pre_level;  // levelsAvailable = depth - GetLevel

  // bootstrap to a target near the top: IR level = depth -> target_gl = 1.
  int       target_level = static_cast<int>(depth);
  CIPHERTEXT output;
  Bootstrap(&output, &input, target_level);

  uint64_t post_level = Level(&output);
  size_t   post_avail = depth - post_level;
  long     delta      = static_cast<long>(post_avail) - static_cast<long>(pre_avail);

  printf("[boot] depth=%zu (fresh_towers=%zu)\n", depth, fresh_towers);
  printf("[boot] pre  GetLevel=%lu  levelsAvailable=%zu\n",
         static_cast<unsigned long>(pre_level), pre_avail);
  printf("[boot] post GetLevel=%lu  levelsAvailable=%zu\n",
         static_cast<unsigned long>(post_level), post_avail);
  printf("[boot] level-refresh delta (post-pre)=%ld\n", delta);

  // record the refresh verdict so the .cxx main can OR it into the final
  // PASS/FAIL; the main additionally checks accuracy.
  g_boot_level_ok = (delta > 0) ? 1 : 0;
  printf("BOOT_LEVEL_OK=%d\n", g_boot_level_ok);

  Set_output_data("output", 0, &output);
  return true;
}
