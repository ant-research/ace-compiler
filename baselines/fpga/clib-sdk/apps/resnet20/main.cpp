// ResNet20 layer3.1: two convolutions, bootstrap and depth-12 polynomial ReLU.
// Only main.cpp/config.mk are application sources; all encoded objects come from ANT.
#include "clib.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
using namespace clib;

static std::vector<double> read_values(const std::string& name, size_t count) {
    std::ifstream file(std::string(CLIB_APP_DATA_DIR) + "/" + name);
    std::vector<double> values; double v;
    while (file >> v) values.push_back(v);
    if (!file.eof() || values.size() != count)
        throw std::runtime_error("incorrect data file: " + name);
    return values;
}

// NCHW: one block = 8 channels * 8 rows * 8 columns = 512 slots.
// Every diagonal includes the spatial padding mask.
static std::vector<double> pack_weight(const std::vector<double>& weights,
                                      int output_block, int input_block, int step) {
    for (int k = 0; k < 8; ++k)
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx) {
                if ((64*k + 8*dy + dx + 512) % 512 != step) continue;
                std::vector<double> values(512);
                for (int j = 0; j < 512; ++j) {
                    int co=j/64, h=(j%64)/8, w=j%8, ci=(co+k)%8;
                    size_t index=(((output_block*8+co)*64+input_block*8+ci)*3+dy+1)*3+dx+1;
                    values[j]=(h+dy>=0 && h+dy<8 && w+dx>=0 && w+dx<8) ? weights[index] : 0.0;
                }
                return values;
            }
    throw std::runtime_error("invalid convolution shift");
}

void Main_graph(CONTEXT ctx) {
    const auto info = Context_info(ctx);
    if (info.ring_degree != 1024 || info.q_limbs != 62)
        throw std::runtime_error("this example requires N1024/Q62");
    const auto weights1=read_values("conv1.weight",64*64*9);
    const auto weights2=read_values("conv2.weight",64*64*9);
    const auto bias1=read_values("conv1.bias",64), bias2=read_values("conv2.bias",64);
    const auto input_values=read_values("sample0.input",4096);
    const auto reference1=read_values("sample0.conv1",4096);
    const auto reference_relu=read_values("sample0.relu",4096);
    const auto reference2=read_values("sample0.conv2",4096);
    const auto exact_relu=read_values("sample0.exact_relu",4096);
    const auto exact_conv2=read_values("sample0.exact_conv2",4096);
    std::vector<double> values(512);
    CIPHER input[8]{}, conv1[8]{}, refreshed[8]{}, activated[8]{}, conv2[8]{};
    CIPHER rotations[8 * 72]{};
    CIPHER relu_tmp[99]{};
    CIPHER sum = nullptr, next_sum = nullptr, term = nullptr, scaled = nullptr;
    CIPHER product2 = nullptr;

    CIPHER3 product3 = nullptr;
    PLAIN plain = nullptr;
    for (int i=0; i<8; ++i) {
        input[i]=Encrypt_input(ctx,("input_"+std::to_string(i)).c_str(),input_values.data()+i*512,512,4);
        conv1[i]=New_ciph(ctx);refreshed[i]=New_ciph(ctx);
        activated[i]=New_ciph(ctx);conv2[i]=New_ciph(ctx);
    }
    sum=New_ciph(ctx);next_sum=New_ciph(ctx);term=New_ciph(ctx);scaled=New_ciph(ctx);
    product2=New_ciph(ctx);product3=New_ciph3(ctx);
    std::set<int> shifts;
    for(int k=0;k<8;++k)for(int dy=-1;dy<=1;++dy)for(int dx=-1;dx<=1;++dx)
        shifts.insert((64*k+8*dy+dx+512)%512);
    std::vector<int> steps(shifts.begin(),shifts.end());
        // Conv1: 64 -> 64 channels, 8x8, 3x3, stride=1, zero padding=1.
        // ANT plaintext diagonals already contain fused BN weights and boundary masks.
        Begin_stage(ctx, "conv1");
        for (int i = 0; i < 8; ++i) {
            for (int k = 0; k < 72; ++k) {
                rotations[i * 72 + k] = New_ciph(ctx);
                Rotate_ciph(ctx, rotations[i * 72 + k], input[i], steps[k]);
            }
        }
        for (int o = 0; o < 8; ++o) {
            for (int i = 0; i < 8; ++i) {
                for (int k = 0; k < 72; ++k) {
                    values = pack_weight(weights1,o,i,steps[k]);
                    const std::string name="conv1/w_"+std::to_string(o)+"_"+std::to_string(i)+"_"+std::to_string(steps[k]);
                    plain=Encode_plain(ctx,name.c_str(),values.data(),512,4);
                    Mul_plain(ctx, term, rotations[i * 72 + k], plain);
                    Free_plain(plain);
                    if (i == 0 && k == 0) {
                        std::swap(sum, term);  // Move the first term into the accumulator.
                    } else {
                        Add_ciph(ctx, next_sum, sum, term);
                        std::swap(sum, next_sum);
                    }
                }
            }
            Rescale_ciph(ctx, scaled, sum);  // Q limbs: 4 -> 2.
            for(int j=0;j<512;++j)values[j]=bias1[o*8+j/64];
            plain=Encode_plain(ctx,("conv1/bias_"+std::to_string(o)).c_str(),values.data(),512,2);
            Add_plain(ctx, conv1[o], scaled, plain);
            Free_plain(plain);
            std::printf("conv1 output_block=%d q_limbs=%u\n", o, Cipher_info(ctx, conv1[o]).q_limbs);
            std::fflush(stdout);
        }
        Free_ciph_array(rotations, 8 * 72);
        End_stage(ctx);
        for (int i = 0; i < 8; ++i)
            Save_output(ctx,("conv1_"+std::to_string(i)).c_str(),conv1[i],reference1.data()+i*512,512,1e-3);

        {
            // Bootstrap: eight independent ciphertexts, 2 -> 32 Q limbs.
            Begin_stage(ctx, "bootstrap");
            for (int i = 0; i < 8; ++i) {
                Bootstrap(ctx, refreshed[i], conv1[i]);
                std::printf("bootstrap output_block=%d q_limbs=%u\n", i, Cipher_info(ctx, refreshed[i]).q_limbs);
                std::fflush(stdout);
            }
            End_stage(ctx);
            for (int i = 0; i < 8; ++i)
                Save_output(ctx,("bootstrap_"+std::to_string(i)).c_str(),refreshed[i],reference1.data()+i*512,512,1e-3);

            // ReLU(t) ~= t * (1 + p13(p15(p7(t)))) / 2.
            // Coefficients are applied in low-degree branches of the decomposition.
            // Depths: p7=3, p15=4, p13=4, final product=1 (12 total).
            // Q limbs: 32 -> 26 -> 18 -> 10 -> 8. Normalization is fused into conv1.
            Begin_stage(ctx, "relu");
            for (int i = 0; i < 8; ++i) {
                for (CIPHER& t : relu_tmp) t = New_ciph(ctx);
                Copy_ciph(ctx, relu_tmp[0], refreshed[i]);
                // Q=30: mul.
                Mul_ciph(ctx, product3, relu_tmp[0], relu_tmp[0]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[1], product2);
                // Q=30: integer.
                Mul_Integer(ctx, relu_tmp[2], relu_tmp[1], 2);
                // Q=30: add_plain.
                std::fill(values.begin(),values.end(),-1);
                plain=Encode_plain(ctx,"relu/minus_one_30",values.data(),512,30);
                Add_plain(ctx, relu_tmp[3], relu_tmp[2], plain);
                Free_plain(plain);
                // Q=28: mul.
                Mul_ciph(ctx, product3, relu_tmp[3], relu_tmp[3]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[4], product2);
                // Q=28: integer.
                Mul_Integer(ctx, relu_tmp[5], relu_tmp[4], 2);
                // Q=28: add_plain.
                std::fill(values.begin(),values.end(),-1);
                plain=Encode_plain(ctx,"relu/minus_one_28",values.data(),512,28);
                Add_plain(ctx, relu_tmp[6], relu_tmp[5], plain);
                Free_plain(plain);
                // Q=30: pmul.
                std::fill(values.begin(),values.end(),-3.8091994324966305);
                plain=Encode_plain(ctx,"relu/coef_2",values.data(),512,32);
                Mul_plain(ctx, term, relu_tmp[0], plain);
                Rescale_ciph(ctx, relu_tmp[7], term);
                Free_plain(plain);
                // Q=28: mul.
                Mul_ciph(ctx, product3, relu_tmp[3], relu_tmp[7]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[8], product2);
                // Q=30: pmul.
                std::fill(values.begin(),values.end(),2.4609408687709102);
                plain=Encode_plain(ctx,"relu/coef_3",values.data(),512,32);
                Mul_plain(ctx, term, relu_tmp[0], plain);
                Rescale_ciph(ctx, relu_tmp[9], term);
                Free_plain(plain);
                // Q=28: modswitch.
                Modswitch(ctx, relu_tmp[10], relu_tmp[9], 28);
                // Q=28: add.
                Add_ciph(ctx, relu_tmp[11], relu_tmp[8], relu_tmp[10]);
                // Q=26: mul.
                Mul_ciph(ctx, product3, relu_tmp[6], relu_tmp[11]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[12], product2);
                // Q=30: pmul.
                std::fill(values.begin(),values.end(),-1.4303047945436642);
                plain=Encode_plain(ctx,"relu/coef_4",values.data(),512,32);
                Mul_plain(ctx, term, relu_tmp[0], plain);
                Rescale_ciph(ctx, relu_tmp[13], term);
                Free_plain(plain);
                // Q=28: mul.
                Mul_ciph(ctx, product3, relu_tmp[3], relu_tmp[13]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[14], product2);
                // Q=30: pmul.
                std::fill(values.begin(),values.end(),2.9446619353537646);
                plain=Encode_plain(ctx,"relu/coef_5",values.data(),512,32);
                Mul_plain(ctx, term, relu_tmp[0], plain);
                Rescale_ciph(ctx, relu_tmp[15], term);
                Free_plain(plain);
                // Q=28: modswitch.
                Modswitch(ctx, relu_tmp[16], relu_tmp[15], 28);
                // Q=28: add.
                Add_ciph(ctx, relu_tmp[17], relu_tmp[14], relu_tmp[16]);
                // Q=26: modswitch.
                Modswitch(ctx, relu_tmp[18], relu_tmp[17], 26);
                // Q=26: add.
                Add_ciph(ctx, relu_tmp[19], relu_tmp[12], relu_tmp[18]);
                if (Cipher_info(ctx, relu_tmp[19]).q_limbs != 26)
                    throw std::runtime_error("p7 level mismatch");
                std::printf("relu block=%d p7_q_limbs=%u\n", i, Cipher_info(ctx, relu_tmp[19]).q_limbs);
                // Q=24: mul.
                Mul_ciph(ctx, product3, relu_tmp[19], relu_tmp[19]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[20], product2);
                // Q=24: integer.
                Mul_Integer(ctx, relu_tmp[21], relu_tmp[20], 2);
                // Q=24: add_plain.
                std::fill(values.begin(),values.end(),-1);
                plain=Encode_plain(ctx,"relu/minus_one_24",values.data(),512,24);
                Add_plain(ctx, relu_tmp[22], relu_tmp[21], plain);
                Free_plain(plain);
                // Q=22: mul.
                Mul_ciph(ctx, product3, relu_tmp[22], relu_tmp[22]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[23], product2);
                // Q=22: integer.
                Mul_Integer(ctx, relu_tmp[24], relu_tmp[23], 2);
                // Q=22: add_plain.
                std::fill(values.begin(),values.end(),-1);
                plain=Encode_plain(ctx,"relu/minus_one_22",values.data(),512,22);
                Add_plain(ctx, relu_tmp[25], relu_tmp[24], plain);
                Free_plain(plain);
                // Q=20: mul.
                Mul_ciph(ctx, product3, relu_tmp[25], relu_tmp[25]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[26], product2);
                // Q=20: integer.
                Mul_Integer(ctx, relu_tmp[27], relu_tmp[26], 2);
                // Q=20: add_plain.
                std::fill(values.begin(),values.end(),-1);
                plain=Encode_plain(ctx,"relu/minus_one_20",values.data(),512,20);
                Add_plain(ctx, relu_tmp[28], relu_tmp[27], plain);
                Free_plain(plain);
                // Q=24: pmul.
                std::fill(values.begin(),values.end(),-0.00016002805372508758);
                plain=Encode_plain(ctx,"relu/coef_9",values.data(),512,26);
                Mul_plain(ctx, term, relu_tmp[19], plain);
                Rescale_ciph(ctx, relu_tmp[29], term);
                Free_plain(plain);
                // Q=22: mul.
                Mul_ciph(ctx, product3, relu_tmp[22], relu_tmp[29]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[30], product2);
                // Q=24: pmul.
                std::fill(values.begin(),values.end(),0.0030324784741275579);
                plain=Encode_plain(ctx,"relu/coef_10",values.data(),512,26);
                Mul_plain(ctx, term, relu_tmp[19], plain);
                Rescale_ciph(ctx, relu_tmp[31], term);
                Free_plain(plain);
                // Q=22: modswitch.
                Modswitch(ctx, relu_tmp[32], relu_tmp[31], 22);
                // Q=22: add.
                Add_ciph(ctx, relu_tmp[33], relu_tmp[30], relu_tmp[32]);
                // Q=20: mul.
                Mul_ciph(ctx, product3, relu_tmp[25], relu_tmp[33]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[34], product2);
                // Q=24: pmul.
                std::fill(values.begin(),values.end(),0.13658283611943375);
                plain=Encode_plain(ctx,"relu/coef_11",values.data(),512,26);
                Mul_plain(ctx, term, relu_tmp[19], plain);
                Rescale_ciph(ctx, relu_tmp[35], term);
                Free_plain(plain);
                // Q=24: modswitch.
                Modswitch(ctx, relu_tmp[36], relu_tmp[19], 24);
                // Q=22: mul.
                Mul_ciph(ctx, product3, relu_tmp[36], relu_tmp[22]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[37], product2);
                // Q=22: integer.
                Mul_Integer(ctx, relu_tmp[38], relu_tmp[37], 2);
                // Q=22: modswitch.
                Modswitch(ctx, relu_tmp[39], relu_tmp[19], 22);
                // Q=22: sub.
                Sub_ciph(ctx, relu_tmp[40], relu_tmp[38], relu_tmp[39]);
                // Q=20: pmul.
                std::fill(values.begin(),values.end(),-0.022197358582167027);
                plain=Encode_plain(ctx,"relu/coef_12",values.data(),512,22);
                Mul_plain(ctx, term, relu_tmp[40], plain);
                Rescale_ciph(ctx, relu_tmp[41], term);
                Free_plain(plain);
                // Q=20: modswitch.
                Modswitch(ctx, relu_tmp[42], relu_tmp[35], 20);
                // Q=20: add.
                Add_ciph(ctx, relu_tmp[43], relu_tmp[42], relu_tmp[41]);
                // Q=20: add.
                Add_ciph(ctx, relu_tmp[44], relu_tmp[34], relu_tmp[43]);
                // Q=18: mul.
                Mul_ciph(ctx, product3, relu_tmp[28], relu_tmp[44]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[45], product2);
                // Q=24: pmul.
                std::fill(values.begin(),values.end(),0.56852311346089535);
                plain=Encode_plain(ctx,"relu/coef_13",values.data(),512,26);
                Mul_plain(ctx, term, relu_tmp[19], plain);
                Rescale_ciph(ctx, relu_tmp[46], term);
                Free_plain(plain);
                // Q=20: pmul.
                std::fill(values.begin(),values.end(),-0.55587853162151368);
                plain=Encode_plain(ctx,"relu/coef_14",values.data(),512,22);
                Mul_plain(ctx, term, relu_tmp[40], plain);
                Rescale_ciph(ctx, relu_tmp[47], term);
                Free_plain(plain);
                // Q=20: modswitch.
                Modswitch(ctx, relu_tmp[48], relu_tmp[46], 20);
                // Q=20: add.
                Add_ciph(ctx, relu_tmp[49], relu_tmp[48], relu_tmp[47]);
                // Q=20: modswitch.
                Modswitch(ctx, relu_tmp[50], relu_tmp[25], 20);
                // Q=18: mul.
                Mul_ciph(ctx, product3, relu_tmp[50], relu_tmp[49]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[51], product2);
                // Q=24: pmul.
                std::fill(values.begin(),values.end(),1.614771079042868);
                plain=Encode_plain(ctx,"relu/coef_15",values.data(),512,26);
                Mul_plain(ctx, term, relu_tmp[19], plain);
                Rescale_ciph(ctx, relu_tmp[52], term);
                Free_plain(plain);
                // Q=20: pmul.
                std::fill(values.begin(),values.end(),-0.61640835832945129);
                plain=Encode_plain(ctx,"relu/coef_16",values.data(),512,22);
                Mul_plain(ctx, term, relu_tmp[40], plain);
                Rescale_ciph(ctx, relu_tmp[53], term);
                Free_plain(plain);
                // Q=20: modswitch.
                Modswitch(ctx, relu_tmp[54], relu_tmp[52], 20);
                // Q=20: add.
                Add_ciph(ctx, relu_tmp[55], relu_tmp[54], relu_tmp[53]);
                // Q=18: modswitch.
                Modswitch(ctx, relu_tmp[56], relu_tmp[55], 18);
                // Q=18: add.
                Add_ciph(ctx, relu_tmp[57], relu_tmp[51], relu_tmp[56]);
                // Q=18: add.
                Add_ciph(ctx, relu_tmp[58], relu_tmp[45], relu_tmp[57]);
                if (Cipher_info(ctx, relu_tmp[58]).q_limbs != 18)
                    throw std::runtime_error("p15 level mismatch");
                std::printf("relu block=%d p15_q_limbs=%u\n", i, Cipher_info(ctx, relu_tmp[58]).q_limbs);
                // Q=16: mul.
                Mul_ciph(ctx, product3, relu_tmp[58], relu_tmp[58]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[59], product2);
                // Q=16: integer.
                Mul_Integer(ctx, relu_tmp[60], relu_tmp[59], 2);
                // Q=16: add_plain.
                std::fill(values.begin(),values.end(),-1);
                plain=Encode_plain(ctx,"relu/minus_one_16",values.data(),512,16);
                Add_plain(ctx, relu_tmp[61], relu_tmp[60], plain);
                Free_plain(plain);
                // Q=14: mul.
                Mul_ciph(ctx, product3, relu_tmp[61], relu_tmp[61]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[62], product2);
                // Q=14: integer.
                Mul_Integer(ctx, relu_tmp[63], relu_tmp[62], 2);
                // Q=14: add_plain.
                std::fill(values.begin(),values.end(),-1);
                plain=Encode_plain(ctx,"relu/minus_one_14",values.data(),512,14);
                Add_plain(ctx, relu_tmp[64], relu_tmp[63], plain);
                Free_plain(plain);
                // Q=12: mul.
                Mul_ciph(ctx, product3, relu_tmp[64], relu_tmp[64]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[65], product2);
                // Q=12: integer.
                Mul_Integer(ctx, relu_tmp[66], relu_tmp[65], 2);
                // Q=12: add_plain.
                std::fill(values.begin(),values.end(),-1);
                plain=Encode_plain(ctx,"relu/minus_one_12",values.data(),512,12);
                Add_plain(ctx, relu_tmp[67], relu_tmp[66], plain);
                Free_plain(plain);
                // Q=16: pmul.
                std::fill(values.begin(),values.end(),0.00011111191620300779);
                plain=Encode_plain(ctx,"relu/coef_20",values.data(),512,18);
                Mul_plain(ctx, term, relu_tmp[58], plain);
                Rescale_ciph(ctx, relu_tmp[68], term);
                Free_plain(plain);
                // Q=14: modswitch.
                Modswitch(ctx, relu_tmp[69], relu_tmp[68], 14);
                // Q=12: mul.
                Mul_ciph(ctx, product3, relu_tmp[64], relu_tmp[69]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[70], product2);
                // Q=16: pmul.
                std::fill(values.begin(),values.end(),0.0065079951902107308);
                plain=Encode_plain(ctx,"relu/coef_21",values.data(),512,18);
                Mul_plain(ctx, term, relu_tmp[58], plain);
                Rescale_ciph(ctx, relu_tmp[71], term);
                Free_plain(plain);
                // Q=16: modswitch.
                Modswitch(ctx, relu_tmp[72], relu_tmp[58], 16);
                // Q=14: mul.
                Mul_ciph(ctx, product3, relu_tmp[72], relu_tmp[61]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[73], product2);
                // Q=14: integer.
                Mul_Integer(ctx, relu_tmp[74], relu_tmp[73], 2);
                // Q=14: modswitch.
                Modswitch(ctx, relu_tmp[75], relu_tmp[58], 14);
                // Q=14: sub.
                Sub_ciph(ctx, relu_tmp[76], relu_tmp[74], relu_tmp[75]);
                // Q=12: pmul.
                std::fill(values.begin(),values.end(),-0.00093710692703472703);
                plain=Encode_plain(ctx,"relu/coef_22",values.data(),512,14);
                Mul_plain(ctx, term, relu_tmp[76], plain);
                Rescale_ciph(ctx, relu_tmp[77], term);
                Free_plain(plain);
                // Q=12: modswitch.
                Modswitch(ctx, relu_tmp[78], relu_tmp[71], 12);
                // Q=12: add.
                Add_ciph(ctx, relu_tmp[79], relu_tmp[78], relu_tmp[77]);
                // Q=12: add.
                Add_ciph(ctx, relu_tmp[80], relu_tmp[70], relu_tmp[79]);
                // Q=10: mul.
                Mul_ciph(ctx, product3, relu_tmp[67], relu_tmp[80]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[81], product2);
                // Q=16: pmul.
                std::fill(values.begin(),values.end(),0.10567447181736148);
                plain=Encode_plain(ctx,"relu/coef_23",values.data(),512,18);
                Mul_plain(ctx, term, relu_tmp[58], plain);
                Rescale_ciph(ctx, relu_tmp[82], term);
                Free_plain(plain);
                // Q=12: pmul.
                std::fill(values.begin(),values.end(),-0.036910639403499486);
                plain=Encode_plain(ctx,"relu/coef_24",values.data(),512,14);
                Mul_plain(ctx, term, relu_tmp[76], plain);
                Rescale_ciph(ctx, relu_tmp[83], term);
                Free_plain(plain);
                // Q=12: modswitch.
                Modswitch(ctx, relu_tmp[84], relu_tmp[82], 12);
                // Q=12: add.
                Add_ciph(ctx, relu_tmp[85], relu_tmp[84], relu_tmp[83]);
                // Q=12: modswitch.
                Modswitch(ctx, relu_tmp[86], relu_tmp[64], 12);
                // Q=10: mul.
                Mul_ciph(ctx, product3, relu_tmp[86], relu_tmp[85]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[87], product2);
                // Q=16: pmul.
                std::fill(values.begin(),values.end(),0.63341398437092888);
                plain=Encode_plain(ctx,"relu/coef_25",values.data(),512,18);
                Mul_plain(ctx, term, relu_tmp[58], plain);
                Rescale_ciph(ctx, relu_tmp[88], term);
                Free_plain(plain);
                // Q=12: pmul.
                std::fill(values.begin(),values.end(),-0.20785971588109659);
                plain=Encode_plain(ctx,"relu/coef_26",values.data(),512,14);
                Mul_plain(ctx, term, relu_tmp[76], plain);
                Rescale_ciph(ctx, relu_tmp[89], term);
                Free_plain(plain);
                // Q=12: modswitch.
                Modswitch(ctx, relu_tmp[90], relu_tmp[88], 12);
                // Q=12: add.
                Add_ciph(ctx, relu_tmp[91], relu_tmp[90], relu_tmp[89]);
                // Q=10: modswitch.
                Modswitch(ctx, relu_tmp[92], relu_tmp[91], 10);
                // Q=10: add.
                Add_ciph(ctx, relu_tmp[93], relu_tmp[87], relu_tmp[92]);
                // Q=10: add.
                Add_ciph(ctx, relu_tmp[94], relu_tmp[81], relu_tmp[93]);
                if (Cipher_info(ctx, relu_tmp[94]).q_limbs != 10)
                    throw std::runtime_error("p13 level mismatch");
                std::printf("relu block=%d p13_q_limbs=%u\n", i, Cipher_info(ctx, relu_tmp[94]).q_limbs);
                // Q=10: modswitch.
                Modswitch(ctx, relu_tmp[95], relu_tmp[0], 10);
                // Q=8: mul.
                Mul_ciph(ctx, product3, relu_tmp[95], relu_tmp[94]);
                Relin(ctx, product2, product3);
                Rescale_ciph(ctx, relu_tmp[96], product2);
                // Q=8: pmul.
                std::fill(values.begin(),values.end(),0.5);
                plain=Encode_plain(ctx,"relu/coef_27",values.data(),512,10);
                Mul_plain(ctx, term, relu_tmp[95], plain);
                Rescale_ciph(ctx, relu_tmp[97], term);
                Free_plain(plain);
                // Q=8: add.
                Add_ciph(ctx, relu_tmp[98], relu_tmp[96], relu_tmp[97]);
                Copy_ciph(ctx, activated[i], relu_tmp[98]);
                Free_ciph_array(relu_tmp, 99);
                std::printf("relu output_block=%d q_limbs=%u\n", i, Cipher_info(ctx, activated[i]).q_limbs);
                std::fflush(stdout);
            }
            End_stage(ctx);
            for (int i = 0; i < 8; ++i)
                Save_output(ctx,("relu_"+std::to_string(i)).c_str(),activated[i],reference_relu.data()+i*512,512,1e-3);

            // Conv2: 64 -> 64 channels, 8x8, 3x3, stride=1, zero padding=1.
            // ANT plaintext diagonals already contain fused BN weights and boundary masks.
            Begin_stage(ctx, "conv2");
            for (int i = 0; i < 8; ++i) {
                for (int k = 0; k < 72; ++k) {
                    rotations[i * 72 + k] = New_ciph(ctx);
                    Rotate_ciph(ctx, rotations[i * 72 + k], activated[i], steps[k]);
                }
            }
            for (int o = 0; o < 8; ++o) {
                for (int i = 0; i < 8; ++i) {
                    for (int k = 0; k < 72; ++k) {
                        values = pack_weight(weights2,o,i,steps[k]);
                    const std::string name="conv2/w_"+std::to_string(o)+"_"+std::to_string(i)+"_"+std::to_string(steps[k]);
                    plain=Encode_plain(ctx,name.c_str(),values.data(),512,8);
                        Mul_plain(ctx, term, rotations[i * 72 + k], plain);
                        Free_plain(plain);
                        if (i == 0 && k == 0) {
                            std::swap(sum, term);  // Move the first term into the accumulator.
                        } else {
                            Add_ciph(ctx, next_sum, sum, term);
                            std::swap(sum, next_sum);
                        }
                    }
                }
                Rescale_ciph(ctx, scaled, sum);  // Q limbs: 8 -> 6.
                for(int j=0;j<512;++j)values[j]=bias2[o*8+j/64];
            plain=Encode_plain(ctx,("conv2/bias_"+std::to_string(o)).c_str(),values.data(),512,6);
                Add_plain(ctx, conv2[o], scaled, plain);
                Free_plain(plain);
                std::printf("conv2 output_block=%d q_limbs=%u\n", o, Cipher_info(ctx, conv2[o]).q_limbs);
                std::fflush(stdout);
            }
            Free_ciph_array(rotations, 8 * 72);
            End_stage(ctx);
            for (int i = 0; i < 8; ++i)
                Save_output(ctx,("conv2_"+std::to_string(i)).c_str(),conv2[i],reference2.data()+i*512,512,1e-3);
        }
    for(int i=0;i<8;++i) {
        Save_output(ctx,("exact_relu_"+std::to_string(i)).c_str(),activated[i],exact_relu.data()+i*512,512,0.04);
        Save_output(ctx,("exact_conv2_"+std::to_string(i)).c_str(),conv2[i],exact_conv2.data()+i*512,512,0.04);
    }
        Free_plain(plain);
        Free_ciph3(product3);
        Free_ciph_array(input, 8);
        Free_ciph_array(conv1, 8);
        Free_ciph_array(refreshed, 8);
        Free_ciph_array(activated, 8);
        Free_ciph_array(conv2, 8);
        Free_ciph_array(rotations, 8 * 72);
        Free_ciph_array(relu_tmp, 99);
        Free_ciph(sum);
        Free_ciph(next_sum);
        Free_ciph(term);
        Free_ciph(scaled);
        Free_ciph(product2);
}

int main(int argc,char** argv) {
    return Run_application(argc,argv,Main_graph);
}
