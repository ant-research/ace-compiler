// Phase 3: layer0 + stage1 (3 residual blocks, identity shortcut, no downsample).
// route B: N=8192, slots=4096/cipher, layer0/stage1 = 4 output ciphers (16ch 32x32, blk_ch=4).
//
// Numerical chain matches the SEAL ckks program (infer_seal.cpp:ResNet_cifar10_seal_sparse):
//   layer0 : conv(3->16,fold fW/B,fb/B) -> Add_plain(bias) -> Bootstrap -> ReLU
//   stage1 block (k=0..2):
//     conv1(16->16,stride1,fold fW no/B, bias=fb/B) -> Modswitch Q2 -> Bootstrap -> ReLU
//     conv2(16->16,stride1,same fold)               -> Modswitch Q2
//     shortcut = block input (identity)             -> Modswitch Q2
//     Add_ciph(conv2, shortcut) @Q2                  -> Bootstrap -> ReLU -> next block input
//
// conv diagonal formula (general, all stages): step = (ci - co_rel)*HW + dy*W + dx (mod SLOTS),
//   absolute input channel ci; weight mask non-zero only at matching co_rel in block.
// ReLU ported from clib-sdk/apps/resnet20/main.cpp:95-340 (depth 12, Q32->8).
//
// Export objects (Encode_plain / Encrypt_input) carry stable names embedding (j,k,kernel,slot)
// so the ANT-encoded set is deterministic across export/run. Names unique within kind:name.
#include "clib.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
using namespace clib;

static std::vector<double> read_values(const std::string& name, size_t count) {
    std::ifstream file(std::string(CLIB_APP_DATA_DIR) + "/" + name);
    if (!file) throw std::runtime_error("cannot open data file: " + name);
    std::vector<double> values; double v;
    while (file >> v) values.push_back(v);
    if (values.size() != count)
        throw std::runtime_error("bad data file size " + name + ": " + std::to_string(values.size()));
    return values;
}

static constexpr int SLOTS = 4096, HW = 1024, H0 = 32, W0 = 32, BLK_CH = 4;
static constexpr int C0_IN = 3, C0_OUT = 16, N_OUT_BLK = 4;   // layer0
static constexpr int S1C = 16;                                 // stage1 channels (in & out)

// Pack a conv weight slice for stage1 (or layer0) into SLOTS double values.
// ob = output cipher index (0..N_OUT_BLK-1); co_rel = output channel within block (0..BLK_CH-1).
// W has shape [C_OUT, C_IN, 3, 3]; indexing (((out_g*C_IN)+ci)*3+(dy+1))*3+(dx+1).
static std::vector<double> pack_weight(const std::vector<double>& W,
                                        int C_IN, int C_OUT, int ob, int co_rel, int ci, int dy, int dx) {
    std::vector<double> values(SLOTS, 0.0);
    for (int j = 0; j < SLOTS; ++j) {
        int cr = j / HW;
        if (cr != co_rel) continue;
        int h = (j % HW) / W0, w = j % W0;
        if (h + dy < 0 || h + dy >= H0 || w + dx < 0 || w + dx >= W0) continue;
        int out_g = ob * BLK_CH + co_rel;
        if (out_g >= C_OUT) continue;
        size_t idx = (((size_t)out_g * C_IN + ci) * 3 + (dy + 1)) * 3 + (dx + 1);
        values[j] = W[idx];
    }
    return values;
}

// Compute the deterministic set of rotation steps for a conv over channels [0,C_IN).
// step = ((ci - co_rel)*HW + dy*W + dx) mod SLOTS.  Same set for layer0 (C_IN=3) and stage1 (C_IN=16).
static std::vector<int> conv_steps(int C_IN) {
    std::set<int> s;
    for (int co_rel = 0; co_rel < BLK_CH; ++co_rel)
        for (int ci = 0; ci < C_IN; ++ci)
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx)
                    s.insert((((ci - co_rel) * HW + dy * W0 + dx) % SLOTS + SLOTS) % SLOTS);
    return std::vector<int>(s.begin(), s.end());
}

// ReLU(Q32->Q8) on one cipher (post-Bootstrap). Ported verbatim from
// clib-sdk/apps/resnet20/main.cpp relu block; 512->4096, for8->for4 already applied.
static void relu_one(CONTEXT ctx, CIPHER activated, CIPHER x, CIPHER3 product3,
                     CIPHER product2, CIPHER term, CIPHER* relu_tmp, PLAIN& plain,
                     std::vector<double>& values, const std::string& pfx) {
    Copy_ciph(ctx, relu_tmp[0], x);
    Mul_ciph(ctx, product3, relu_tmp[0], relu_tmp[0]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[1], product2);
    Mul_Integer(ctx, relu_tmp[2], relu_tmp[1], 2);
    std::fill(values.begin(), values.end(), -1);
    plain = Encode_plain(ctx, (pfx + "minus_one_30").c_str(), values.data(), SLOTS, 30);
    Add_plain(ctx, relu_tmp[3], relu_tmp[2], plain); Free_plain(plain);
    Mul_ciph(ctx, product3, relu_tmp[3], relu_tmp[3]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[4], product2);
    Mul_Integer(ctx, relu_tmp[5], relu_tmp[4], 2);
    std::fill(values.begin(), values.end(), -1);
    plain = Encode_plain(ctx, (pfx + "minus_one_28").c_str(), values.data(), SLOTS, 28);
    Add_plain(ctx, relu_tmp[6], relu_tmp[5], plain); Free_plain(plain);
    std::fill(values.begin(), values.end(), -3.8091994324966305);
    plain = Encode_plain(ctx, (pfx + "coef_2").c_str(), values.data(), SLOTS, 32);
    Mul_plain(ctx, term, relu_tmp[0], plain); Rescale_ciph(ctx, relu_tmp[7], term); Free_plain(plain);
    Mul_ciph(ctx, product3, relu_tmp[3], relu_tmp[7]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[8], product2);
    std::fill(values.begin(), values.end(), 2.4609408687709102);
    plain = Encode_plain(ctx, (pfx + "coef_3").c_str(), values.data(), SLOTS, 32);
    Mul_plain(ctx, term, relu_tmp[0], plain); Rescale_ciph(ctx, relu_tmp[9], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[10], relu_tmp[9], 28);
    Add_ciph(ctx, relu_tmp[11], relu_tmp[8], relu_tmp[10]);
    Mul_ciph(ctx, product3, relu_tmp[6], relu_tmp[11]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[12], product2);
    std::fill(values.begin(), values.end(), -1.4303047945436642);
    plain = Encode_plain(ctx, (pfx + "coef_4").c_str(), values.data(), SLOTS, 32);
    Mul_plain(ctx, term, relu_tmp[0], plain); Rescale_ciph(ctx, relu_tmp[13], term); Free_plain(plain);
    Mul_ciph(ctx, product3, relu_tmp[3], relu_tmp[13]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[14], product2);
    std::fill(values.begin(), values.end(), 2.9446619353537646);
    plain = Encode_plain(ctx, (pfx + "coef_5").c_str(), values.data(), SLOTS, 32);
    Mul_plain(ctx, term, relu_tmp[0], plain); Rescale_ciph(ctx, relu_tmp[15], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[16], relu_tmp[15], 28);
    Add_ciph(ctx, relu_tmp[17], relu_tmp[14], relu_tmp[16]);
    Modswitch(ctx, relu_tmp[18], relu_tmp[17], 26);
    Add_ciph(ctx, relu_tmp[19], relu_tmp[12], relu_tmp[18]);
    Mul_ciph(ctx, product3, relu_tmp[19], relu_tmp[19]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[20], product2);
    Mul_Integer(ctx, relu_tmp[21], relu_tmp[20], 2);
    std::fill(values.begin(), values.end(), -1);
    plain = Encode_plain(ctx, (pfx + "minus_one_24").c_str(), values.data(), SLOTS, 24);
    Add_plain(ctx, relu_tmp[22], relu_tmp[21], plain); Free_plain(plain);
    Mul_ciph(ctx, product3, relu_tmp[22], relu_tmp[22]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[23], product2);
    Mul_Integer(ctx, relu_tmp[24], relu_tmp[23], 2);
    std::fill(values.begin(), values.end(), -1);
    plain = Encode_plain(ctx, (pfx + "minus_one_22").c_str(), values.data(), SLOTS, 22);
    Add_plain(ctx, relu_tmp[25], relu_tmp[24], plain); Free_plain(plain);
    Mul_ciph(ctx, product3, relu_tmp[25], relu_tmp[25]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[26], product2);
    Mul_Integer(ctx, relu_tmp[27], relu_tmp[26], 2);
    std::fill(values.begin(), values.end(), -1);
    plain = Encode_plain(ctx, (pfx + "minus_one_20").c_str(), values.data(), SLOTS, 20);
    Add_plain(ctx, relu_tmp[28], relu_tmp[27], plain); Free_plain(plain);
    std::fill(values.begin(), values.end(), -0.00016002805372508758);
    plain = Encode_plain(ctx, (pfx + "coef_9").c_str(), values.data(), SLOTS, 26);
    Mul_plain(ctx, term, relu_tmp[19], plain); Rescale_ciph(ctx, relu_tmp[29], term); Free_plain(plain);
    Mul_ciph(ctx, product3, relu_tmp[22], relu_tmp[29]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[30], product2);
    std::fill(values.begin(), values.end(), 0.0030324784741275579);
    plain = Encode_plain(ctx, (pfx + "coef_10").c_str(), values.data(), SLOTS, 26);
    Mul_plain(ctx, term, relu_tmp[19], plain); Rescale_ciph(ctx, relu_tmp[31], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[32], relu_tmp[31], 22);
    Add_ciph(ctx, relu_tmp[33], relu_tmp[30], relu_tmp[32]);
    Mul_ciph(ctx, product3, relu_tmp[25], relu_tmp[33]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[34], product2);
    std::fill(values.begin(), values.end(), 0.13658283611943375);
    plain = Encode_plain(ctx, (pfx + "coef_11").c_str(), values.data(), SLOTS, 26);
    Mul_plain(ctx, term, relu_tmp[19], plain); Rescale_ciph(ctx, relu_tmp[35], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[36], relu_tmp[19], 24);
    Mul_ciph(ctx, product3, relu_tmp[36], relu_tmp[22]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[37], product2);
    Mul_Integer(ctx, relu_tmp[38], relu_tmp[37], 2);
    Modswitch(ctx, relu_tmp[39], relu_tmp[19], 22);
    Sub_ciph(ctx, relu_tmp[40], relu_tmp[38], relu_tmp[39]);
    std::fill(values.begin(), values.end(), -0.022197358582167027);
    plain = Encode_plain(ctx, (pfx + "coef_12").c_str(), values.data(), SLOTS, 22);
    Mul_plain(ctx, term, relu_tmp[40], plain); Rescale_ciph(ctx, relu_tmp[41], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[42], relu_tmp[35], 20);
    Add_ciph(ctx, relu_tmp[43], relu_tmp[42], relu_tmp[41]);
    Add_ciph(ctx, relu_tmp[44], relu_tmp[34], relu_tmp[43]);
    Mul_ciph(ctx, product3, relu_tmp[28], relu_tmp[44]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[45], product2);
    std::fill(values.begin(), values.end(), 0.56852311346089535);
    plain = Encode_plain(ctx, (pfx + "coef_13").c_str(), values.data(), SLOTS, 26);
    Mul_plain(ctx, term, relu_tmp[19], plain); Rescale_ciph(ctx, relu_tmp[46], term); Free_plain(plain);
    std::fill(values.begin(), values.end(), -0.55587853162151368);
    plain = Encode_plain(ctx, (pfx + "coef_14").c_str(), values.data(), SLOTS, 22);
    Mul_plain(ctx, term, relu_tmp[40], plain); Rescale_ciph(ctx, relu_tmp[47], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[48], relu_tmp[46], 20);
    Add_ciph(ctx, relu_tmp[49], relu_tmp[48], relu_tmp[47]);
    Modswitch(ctx, relu_tmp[50], relu_tmp[25], 20);
    Mul_ciph(ctx, product3, relu_tmp[50], relu_tmp[49]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[51], product2);
    std::fill(values.begin(), values.end(), 1.614771079042868);
    plain = Encode_plain(ctx, (pfx + "coef_15").c_str(), values.data(), SLOTS, 26);
    Mul_plain(ctx, term, relu_tmp[19], plain); Rescale_ciph(ctx, relu_tmp[52], term); Free_plain(plain);
    std::fill(values.begin(), values.end(), -0.61640835832945129);
    plain = Encode_plain(ctx, (pfx + "coef_16").c_str(), values.data(), SLOTS, 22);
    Mul_plain(ctx, term, relu_tmp[40], plain); Rescale_ciph(ctx, relu_tmp[53], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[54], relu_tmp[52], 20);
    Add_ciph(ctx, relu_tmp[55], relu_tmp[54], relu_tmp[53]);
    Modswitch(ctx, relu_tmp[56], relu_tmp[55], 18);
    Add_ciph(ctx, relu_tmp[57], relu_tmp[51], relu_tmp[56]);
    Add_ciph(ctx, relu_tmp[58], relu_tmp[45], relu_tmp[57]);
    Mul_ciph(ctx, product3, relu_tmp[58], relu_tmp[58]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[59], product2);
    Mul_Integer(ctx, relu_tmp[60], relu_tmp[59], 2);
    std::fill(values.begin(), values.end(), -1);
    plain = Encode_plain(ctx, (pfx + "minus_one_16").c_str(), values.data(), SLOTS, 16);
    Add_plain(ctx, relu_tmp[61], relu_tmp[60], plain); Free_plain(plain);
    Mul_ciph(ctx, product3, relu_tmp[61], relu_tmp[61]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[62], product2);
    Mul_Integer(ctx, relu_tmp[63], relu_tmp[62], 2);
    std::fill(values.begin(), values.end(), -1);
    plain = Encode_plain(ctx, (pfx + "minus_one_14").c_str(), values.data(), SLOTS, 14);
    Add_plain(ctx, relu_tmp[64], relu_tmp[63], plain); Free_plain(plain);
    Mul_ciph(ctx, product3, relu_tmp[64], relu_tmp[64]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[65], product2);
    Mul_Integer(ctx, relu_tmp[66], relu_tmp[65], 2);
    std::fill(values.begin(), values.end(), -1);
    plain = Encode_plain(ctx, (pfx + "minus_one_12").c_str(), values.data(), SLOTS, 12);
    Add_plain(ctx, relu_tmp[67], relu_tmp[66], plain); Free_plain(plain);
    std::fill(values.begin(), values.end(), 0.00011111191620300779);
    plain = Encode_plain(ctx, (pfx + "coef_20").c_str(), values.data(), SLOTS, 18);
    Mul_plain(ctx, term, relu_tmp[58], plain); Rescale_ciph(ctx, relu_tmp[68], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[69], relu_tmp[68], 14);
    Mul_ciph(ctx, product3, relu_tmp[64], relu_tmp[69]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[70], product2);
    std::fill(values.begin(), values.end(), 0.0065079951902107308);
    plain = Encode_plain(ctx, (pfx + "coef_21").c_str(), values.data(), SLOTS, 18);
    Mul_plain(ctx, term, relu_tmp[58], plain); Rescale_ciph(ctx, relu_tmp[71], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[72], relu_tmp[58], 16);
    Mul_ciph(ctx, product3, relu_tmp[72], relu_tmp[61]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[73], product2);
    Mul_Integer(ctx, relu_tmp[74], relu_tmp[73], 2);
    Modswitch(ctx, relu_tmp[75], relu_tmp[58], 14);
    Sub_ciph(ctx, relu_tmp[76], relu_tmp[74], relu_tmp[75]);
    std::fill(values.begin(), values.end(), -0.00093710692703472703);
    plain = Encode_plain(ctx, (pfx + "coef_22").c_str(), values.data(), SLOTS, 14);
    Mul_plain(ctx, term, relu_tmp[76], plain); Rescale_ciph(ctx, relu_tmp[77], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[78], relu_tmp[71], 12);
    Add_ciph(ctx, relu_tmp[79], relu_tmp[78], relu_tmp[77]);
    Add_ciph(ctx, relu_tmp[80], relu_tmp[70], relu_tmp[79]);
    Mul_ciph(ctx, product3, relu_tmp[67], relu_tmp[80]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[81], product2);
    std::fill(values.begin(), values.end(), 0.10567447181736148);
    plain = Encode_plain(ctx, (pfx + "coef_23").c_str(), values.data(), SLOTS, 18);
    Mul_plain(ctx, term, relu_tmp[58], plain); Rescale_ciph(ctx, relu_tmp[82], term); Free_plain(plain);
    std::fill(values.begin(), values.end(), -0.036910639403499486);
    plain = Encode_plain(ctx, (pfx + "coef_24").c_str(), values.data(), SLOTS, 14);
    Mul_plain(ctx, term, relu_tmp[76], plain); Rescale_ciph(ctx, relu_tmp[83], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[84], relu_tmp[82], 12);
    Add_ciph(ctx, relu_tmp[85], relu_tmp[84], relu_tmp[83]);
    Modswitch(ctx, relu_tmp[86], relu_tmp[64], 12);
    Mul_ciph(ctx, product3, relu_tmp[86], relu_tmp[85]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[87], product2);
    std::fill(values.begin(), values.end(), 0.63341398437092888);
    plain = Encode_plain(ctx, (pfx + "coef_25").c_str(), values.data(), SLOTS, 18);
    Mul_plain(ctx, term, relu_tmp[58], plain); Rescale_ciph(ctx, relu_tmp[88], term); Free_plain(plain);
    std::fill(values.begin(), values.end(), -0.20785971588109659);
    plain = Encode_plain(ctx, (pfx + "coef_26").c_str(), values.data(), SLOTS, 14);
    Mul_plain(ctx, term, relu_tmp[76], plain); Rescale_ciph(ctx, relu_tmp[89], term); Free_plain(plain);
    Modswitch(ctx, relu_tmp[90], relu_tmp[88], 12);
    Add_ciph(ctx, relu_tmp[91], relu_tmp[90], relu_tmp[89]);
    Modswitch(ctx, relu_tmp[92], relu_tmp[91], 10);
    Add_ciph(ctx, relu_tmp[93], relu_tmp[87], relu_tmp[92]);
    Add_ciph(ctx, relu_tmp[94], relu_tmp[81], relu_tmp[93]);
    Modswitch(ctx, relu_tmp[95], relu_tmp[0], 10);
    Mul_ciph(ctx, product3, relu_tmp[95], relu_tmp[94]); Relin(ctx, product2, product3);
    Rescale_ciph(ctx, relu_tmp[96], product2);
    std::fill(values.begin(), values.end(), 0.5);
    plain = Encode_plain(ctx, (pfx + "coef_27").c_str(), values.data(), SLOTS, 10);
    Mul_plain(ctx, term, relu_tmp[95], plain); Rescale_ciph(ctx, relu_tmp[97], term); Free_plain(plain);
    Add_ciph(ctx, relu_tmp[98], relu_tmp[96], relu_tmp[97]);
    Copy_ciph(ctx, activated, relu_tmp[98]);
}

// Convolve: out[cipher] = sum over (co_rel, ci, dy, dx) of Rotate(input_cipher_by_step) * weight.
// 'in_ciphers' = N_OUT_BLK input ciphers (stage1: same input replicated-resolved per ob via absolute ci).
// Each ob's input cipher is `in_ciphers` resolved by absolute channel: input channel ci lives in
// cipher (ci/BLK_CH); since all stage1 blocks have ci,co within [0,16) and BLK_CH=4, absolute
// ci maps to in_ciphers[ci/BLK_CH]. We precompute rotations of EACH input cipher over the full
// step set (the step offsets already encode which absolute input channel they rotate to).
// Returns freshly New_ciph'd conv outputs at Q (caller frees).
static void conv_stage(CONTEXT ctx,
                       CIPHER* out,            // [N_OUT_BLK]
                       const CIPHER* in_ciph,  // [N_OUT_BLK] input ciphers (block-packed 16ch 32x32)
                       const std::vector<double>& W, int C_IN, int C_OUT,
                       const std::vector<double>& bias,     // [C_OUT]
                       const std::vector<int>& steps,
                       uint32_t q_w, uint32_t q_bias,        // weight plane Q (== input Q), bias plane Q (=q_w-2)
                       CIPHER sum, CIPHER next_sum, CIPHER term, CIPHER scaled,
                       PLAIN& plain, std::vector<double>& values,
                       const std::string& pfx) {            // e.g. "s1_0_1/" (block, which conv)
    // rotations of each input cipher, indexed by step.  Only rotate the input ciphers
    // actually referenced by absolute input channels ci -> in_b = ci/BLK_CH.
    int n_in = (C_IN + BLK_CH - 1) / BLK_CH;       // layer0: 1; stage1: 4
    std::vector<std::vector<CIPHER>> rot(N_OUT_BLK, std::vector<CIPHER>(steps.size(), nullptr));
    for (int b = 0; b < n_in; ++b)
        for (size_t s = 0; s < steps.size(); ++s) {
            rot[b][s] = New_ciph(ctx);
            Rotate_ciph(ctx, rot[b][s], in_ciph[b], steps[s]);
        }
    for (int b = n_in; b < N_OUT_BLK; ++b)
        for (size_t s = 0; s < steps.size(); ++s)
            rot[b][s] = nullptr;
    for (int ob = 0; ob < N_OUT_BLK; ++ob) {
        bool first = true;
        for (int co_rel = 0; co_rel < BLK_CH; ++co_rel) {
            if (ob * BLK_CH + co_rel >= C_OUT) continue;
            for (int ci = 0; ci < C_IN; ++ci)
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        int step = (((ci - co_rel) * HW + dy * W0 + dx) % SLOTS + SLOTS) % SLOTS;
                        size_t sidx = std::lower_bound(steps.begin(), steps.end(), step) - steps.begin();
                        int in_b = ci / BLK_CH;          // absolute input channel -> input cipher
                        auto w = pack_weight(W, C_IN, C_OUT, ob, co_rel, ci, dy, dx);
                        std::string name = pfx + "w_" + std::to_string(ob) + "_" + std::to_string(co_rel)
                                         + "_" + std::to_string(ci) + "_" + std::to_string(dy+1)
                                         + std::to_string(dx+1);
                        plain = Encode_plain(ctx, name.c_str(), w.data(), SLOTS, q_w);
                        Mul_plain(ctx, term, rot[in_b][sidx], plain); Free_plain(plain);
                        if (first) { std::swap(sum, term); first = false; }
                        else { Add_ciph(ctx, next_sum, sum, term); std::swap(sum, next_sum); }
                    }
        }
        Rescale_ciph(ctx, scaled, sum);                 // q_w -> q_w-2
        std::vector<double> b(SLOTS, 0.0);
        for (int j = 0; j < SLOTS; ++j) { int og = ob*BLK_CH + j/HW; b[j] = (og < C_OUT) ? bias[og] : 0.0; }
        std::string bname = pfx + "bias_" + std::to_string(ob);
        plain = Encode_plain(ctx, bname.c_str(), b.data(), SLOTS, q_bias);
        Add_plain(ctx, out[ob], scaled, plain); Free_plain(plain);
    }
    // free rotations
    for (int b = 0; b < N_OUT_BLK; ++b)
        for (size_t s = 0; s < steps.size(); ++s) Free_ciph(rot[b][s]);
}

void Main_graph(CONTEXT ctx) {
    const auto info = Context_info(ctx);
    if (info.ring_degree != 8192 || info.q_limbs != 62)
        throw std::runtime_error("this app requires N8192/Q62");

    // ---- load data ----
    auto W0   = read_values("layer0.conv.weight", (size_t)C0_OUT * C0_IN * 9);
    auto bias0= read_values("layer0.conv.bias", C0_OUT);
    auto in_raw = read_values("input.img", SLOTS);
    auto ref_conv0 = read_values("layer0.conv.ref", (size_t)N_OUT_BLK * SLOTS);
    auto ref_relu0 = read_values("layer0.relu.ref", (size_t)N_OUT_BLK * SLOTS);
    // stage1 blocks(j=0) k=0..2: conv1 + conv2 weights/bias + refs
    std::vector<std::vector<double>> W1(3), b1(3), W2(3), b2(3);
    std::vector<std::vector<double>> rc1(3), rr1(3), rc2(3), rr2(3);
    for (int k = 0; k < 3; ++k) {
        W1[k]  = read_values(("s1_"+std::to_string(k)+".conv1.weight"), (size_t)S1C*S1C*9);
        b1[k]  = read_values(("s1_"+std::to_string(k)+".conv1.bias"), S1C);
        W2[k]  = read_values(("s1_"+std::to_string(k)+".conv2.weight"), (size_t)S1C*S1C*9);
        b2[k]  = read_values(("s1_"+std::to_string(k)+".conv2.bias"), S1C);
        rc1[k] = read_values(("s1_"+std::to_string(k)+".conv1.ref"), (size_t)N_OUT_BLK*SLOTS);
        rr1[k] = read_values(("s1_"+std::to_string(k)+".relu1.ref"),  (size_t)N_OUT_BLK*SLOTS);
        rc2[k] = read_values(("s1_"+std::to_string(k)+".conv2.ref"), (size_t)N_OUT_BLK*SLOTS);
        rr2[k] = read_values(("s1_"+std::to_string(k)+".relu2.ref"),  (size_t)N_OUT_BLK*SLOTS);
    }

    std::vector<int> steps = conv_steps(S1C);  // also covers layer0 (subset) — reuse same set

    CIPHER input = Encrypt_input(ctx, "input", in_raw.data(), SLOTS, 4);

    // shared scratch
    CIPHER sum=New_ciph(ctx), next_sum=New_ciph(ctx), term=New_ciph(ctx), scaled=New_ciph(ctx);
    CIPHER product2=New_ciph(ctx); CIPHER3 product3=New_ciph3(ctx);
    CIPHER relu_tmp[99]; for (auto&t:relu_tmp) t=nullptr;
    PLAIN plain=nullptr;
    std::vector<double> values(SLOTS);

    // ============================================================
    // ---- layer0 conv (3->16) ----
    // ============================================================
    CIPHER conv0[N_OUT_BLK], refresh0[N_OUT_BLK], activ0[N_OUT_BLK];
    for (int o=0;o<N_OUT_BLK;++o){ conv0[o]=New_ciph(ctx); refresh0[o]=New_ciph(ctx); activ0[o]=New_ciph(ctx); }

    Begin_stage(ctx, "layer0_conv");
    conv_stage(ctx, conv0, &input, W0, C0_IN, C0_OUT, bias0, steps, /*q_w=*/4, /*q_bias=*/2,
               sum, next_sum, term, scaled, plain, values, "l0/");
    End_stage(ctx);
    for (int o=0;o<N_OUT_BLK;++o)
        Save_output(ctx,("layer0_conv_"+std::to_string(o)).c_str(),conv0[o],ref_conv0.data()+(size_t)o*SLOTS,SLOTS,1e-3);

    // ---- bootstrap (conv0 already Q2 = bootstrap input Q) ----
    Begin_stage(ctx, "layer0_bootstrap");
    for (int i=0;i<N_OUT_BLK;++i){
        Bootstrap(ctx, refresh0[i], conv0[i]);
        std::printf("layer0_boot i=%d q=%u\n",i,Cipher_info(ctx,refresh0[i]).q_limbs); std::fflush(stdout);
    }
    End_stage(ctx);
    for (int i=0;i<N_OUT_BLK;++i)
        Save_output(ctx,("layer0_boot_"+std::to_string(i)).c_str(),refresh0[i],ref_conv0.data()+(size_t)i*SLOTS,SLOTS,2.0);

    // ---- ReLU (Q32 -> Q8) ----
    Begin_stage(ctx, "layer0_relu");
    for (int i=0;i<N_OUT_BLK;++i){
        for (auto&t:relu_tmp) t=New_ciph(ctx);
        relu_one(ctx, activ0[i], refresh0[i], product3, product2, term, relu_tmp, plain, values, "reluL0/");
        Free_ciph_array(relu_tmp, 99);
        std::printf("layer0_relu i=%d q=%u\n",i,Cipher_info(ctx,activ0[i]).q_limbs); std::fflush(stdout);
    }
    End_stage(ctx);
    for (int i=0;i<N_OUT_BLK;++i)
        Save_output(ctx,("layer0_relu_"+std::to_string(i)).c_str(),activ0[i],ref_relu0.data()+(size_t)i*SLOTS,SLOTS,4e-2);

    // ============================================================
    // ---- stage1: 3 residual blocks (identity shortcut, no downsample) ----
    // ============================================================
#if 1 // stage1 enabled to isolate export failure to layer0
    // block input = previous activ (Q8); 4 ciphers.
    // block input = previous activ (Q8); 4 ciphers.
    CIPHER blk_in[N_OUT_BLK], sc[N_OUT_BLK];
    CIPHER c1[N_OUT_BLK], r1[N_OUT_BLK], c2[N_OUT_BLK], res[N_OUT_BLK], r2[N_OUT_BLK];
    CIPHER sc_ms[N_OUT_BLK], c2_ms[N_OUT_BLK];
    for (int o=0;o<N_OUT_BLK;++o){ blk_in[o]=New_ciph(ctx); sc[o]=New_ciph(ctx);
        c1[o]=New_ciph(ctx); r1[o]=New_ciph(ctx); c2[o]=New_ciph(ctx);
        res[o]=New_ciph(ctx); r2[o]=New_ciph(ctx); sc_ms[o]=New_ciph(ctx); c2_ms[o]=New_ciph(ctx); }

    // seed block_in = layer0 ReLU output (the chain input to stage1 block 0)
    for (int o=0;o<N_OUT_BLK;++o) Copy_ciph(ctx, blk_in[o], activ0[o]);

    for (int k=0;k<3;++k){
        std::string kp = "s1_" + std::to_string(k) + "_";

        // shortcut = block input (identity); save copies (operated on later via Modswitch)
        for (int o=0;o<N_OUT_BLK;++o) Copy_ciph(ctx, sc[o], blk_in[o]);

        // ---- conv1 (16->16, stride1) + bias  :  Q8 -> Q6 ----
        Begin_stage(ctx, ("s1_"+std::to_string(k)+"_conv1").c_str());
        conv_stage(ctx, c1, blk_in, W1[k], S1C, S1C, b1[k], steps, /*q_w=*/8, /*q_bias=*/6,
                   sum, next_sum, term, scaled, plain, values, kp+"c1/");
        End_stage(ctx);
        for (int o=0;o<N_OUT_BLK;++o)
            Save_output(ctx,(kp+"conv1_"+std::to_string(o)).c_str(),c1[o],rc1[k].data()+(size_t)o*SLOTS,SLOTS,4e-2);

        // ---- bootstrap (Q6->Q2) + ReLU (Q32->Q8) ----
        Begin_stage(ctx, ("s1_"+std::to_string(k)+"_relu1").c_str());
        for (int o=0;o<N_OUT_BLK;++o){
            Modswitch(ctx, term, c1[o], 2);
            Bootstrap(ctx, r1[o], term);
            for (auto&t:relu_tmp) t=New_ciph(ctx);
            relu_one(ctx, r1[o], r1[o], product3, product2, term, relu_tmp, plain, values,
                     ("relu"+std::to_string(k)+"a/").c_str());
            Free_ciph_array(relu_tmp, 99);
        }
        End_stage(ctx);
        for (int o=0;o<N_OUT_BLK;++o)
            Save_output(ctx,(kp+"relu1_"+std::to_string(o)).c_str(),r1[o],rr1[k].data()+(size_t)o*SLOTS,SLOTS,4e-2);

        // ---- conv2 (16->16, stride1) + bias : Q8 -> Q6 ; then Modswitch -> Q2 ----
        Begin_stage(ctx, ("s1_"+std::to_string(k)+"_conv2").c_str());
        conv_stage(ctx, c2, r1, W2[k], S1C, S1C, b2[k], steps, /*q_w=*/8, /*q_bias=*/6,
                   sum, next_sum, term, scaled, plain, values, kp+"c2/");
        End_stage(ctx);
        for (int o=0;o<N_OUT_BLK;++o)
            Save_output(ctx,(kp+"conv2_"+std::to_string(o)).c_str(),c2[o],rc2[k].data()+(size_t)o*SLOTS,SLOTS,4e-2);

        // ---- shortcut down to Q2, conv2 down to Q2, Add_ciph, Bootstrap, ReLU ----
        Begin_stage(ctx, ("s1_"+std::to_string(k)+"_addrelu").c_str());
        for (int o=0;o<N_OUT_BLK;++o){
            Modswitch(ctx, c2_ms[o], c2[o], 2);
            Modswitch(ctx, sc_ms[o], sc[o], 2);
            Add_ciph(ctx, res[o], c2_ms[o], sc_ms[o]);   // both deg1 Q2 -> valid
            Bootstrap(ctx, r2[o], res[o]);
            for (auto&t:relu_tmp) t=New_ciph(ctx);
            relu_one(ctx, r2[o], r2[o], product3, product2, term, relu_tmp, plain, values,
                     ("relu"+std::to_string(k)+"b/").c_str());
            Free_ciph_array(relu_tmp, 99);
        }
        End_stage(ctx);
        for (int o=0;o<N_OUT_BLK;++o)
            Save_output(ctx,(kp+"relu2_"+std::to_string(o)).c_str(),r2[o],rr2[k].data()+(size_t)o*SLOTS,SLOTS,4e-2);

        // next block input = this block's ReLU2 output
        for (int o=0;o<N_OUT_BLK;++o) Copy_ciph(ctx, blk_in[o], r2[o]);
        std::printf("stage1 block %d done\n", k); std::fflush(stdout);
    }

    // TODO Phase 4: stage2/3 (downsample shortcut) — route decision pending
    // (route B pure block-packing: downsample needs ~3000+ rotations >512 cap; SEAL uses
    //  multiplex k=2 packing. Decision between hybrid-multiplex-on-stage2/3 vs switch-to-route-A
    //  deferred until stage1 end-to-end PASS + per-block timing measured here.)
#endif

    // ---- free everything ----
    Free_plain(plain); Free_ciph(scaled); Free_ciph(sum); Free_ciph(next_sum); Free_ciph(term);
    Free_ciph(product2); Free_ciph3(product3);
    Free_ciph(input);
    for (int o=0;o<N_OUT_BLK;++o){
        Free_ciph(conv0[o]); Free_ciph(refresh0[o]); Free_ciph(activ0[o]);
#if 1 // stage1 frees
        Free_ciph(blk_in[o]); Free_ciph(sc[o]); Free_ciph(c1[o]); Free_ciph(r1[o]);
        Free_ciph(c2[o]); Free_ciph(res[o]); Free_ciph(r2[o]); Free_ciph(sc_ms[o]); Free_ciph(c2_ms[o]);
#endif
    }
}

int main(int argc, char** argv){ return Run_application(argc,argv,Main_graph); }
