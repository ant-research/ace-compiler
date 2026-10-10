#include "clib.h"
#include <string>
#include <utility>
#include <vector>
using namespace clib;

// Edit this function. The SAME code exports ANT requests and executes the graph.
void Main_graph(CONTEXT ctx) {
    const size_t slots = Context_info(ctx).slots;
    std::vector<double> input(slots), w0(slots, 0.125), w1(slots), w2(slots, -0.0625);
    std::vector<double> reference(slots);
    for (size_t j = 0; j < slots; ++j) {
        input[j] = (int(j % 31) - 15) / 64.0;
        w1[j] = (int(j % 7) - 3) / 64.0;
    }
    for (size_t j = 0; j < slots; ++j)
        reference[j] = w0[j]*input[j] + w1[j]*input[(j+1)%slots] + w2[j]*input[(j+17)%slots];

    CIPHER x = Encrypt_input(ctx, "input", input.data(), slots, 4);
    PLAIN weights[] = {
        Encode_plain(ctx, "w0", w0.data(), slots, 4),
        Encode_plain(ctx, "w1", w1.data(), slots, 4),
        Encode_plain(ctx, "w2", w2.data(), slots, 4)
    };
    CIPHER rot = New_ciph(ctx), product = New_ciph(ctx), sum = New_ciph(ctx);
    CIPHER tmp = New_ciph(ctx), scaled = New_ciph(ctx), refreshed = New_ciph(ctx);

    Begin_stage(ctx, "mvm");
    Mul_plain(ctx, sum, x, weights[0]);
    const int steps[] = {1, 17};
    for (int i = 0; i < 2; ++i) {
        Rotate_ciph(ctx, rot, x, steps[i]);
        Mul_plain(ctx, product, rot, weights[i+1]);
        Add_ciph(ctx, tmp, sum, product);
        std::swap(sum, tmp);
    }
    Rescale_ciph(ctx, scaled, sum); // Q4 -> Q2
    End_stage(ctx);
    Save_output(ctx, "mvm", scaled, reference.data(), slots, 1e-8);

    Begin_stage(ctx, "bootstrap");
    Bootstrap(ctx, refreshed, scaled, 6);
    End_stage(ctx);
    Save_output(ctx, "bootstrap", refreshed, reference.data(), slots, 5e-4);

    for (auto& p : weights) Free_plain(p);
    Free_ciph(x); Free_ciph(rot); Free_ciph(product); Free_ciph(sum);
    Free_ciph(tmp); Free_ciph(scaled); Free_ciph(refreshed);
}

int main(int argc, char** argv) {
    return Run_application(argc, argv, Main_graph);
}
