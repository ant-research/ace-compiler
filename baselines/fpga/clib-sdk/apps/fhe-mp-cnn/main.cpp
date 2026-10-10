// FHE-MP-CNN (homomorphic ResNet on CIFAR, cpu-ckks/single-key/cnn) as a
// clib-sdk application, following the apps/ convention (README §使用流程,
// docs/APPLICATION.md): main() hands argv straight to clib::Run_application
// and Main_graph runs the inference graph. The Makefile invokes
//
//   .build/app --clib-mode export|run --N 65536 --Q 62 --work work --output result
//
// so the SDK owns the mode/context/argument handling; this file only picks
// up the model selection (CNN_LAYERS/CNN_DATASET/CNN_START/CNN_END, exported
// by config.mk) and runs the graph.
//
// The graph implementation is not written against clib directly: the CNN is
// existing SEAL-3.6 code (the clib-adapted staged copy prepared by
// scripts/build_cnn_clib.py; see config.mk for how the staged sources are
// linked into this application). Main_graph adopts the clib context into
// the SEAL compatibility layer (../../../clib-seal, seal::adopt_clib_context),
// after which every SEAL-level encode/encrypt/evaluate of the CNN reaches
// the real clib backend:
//
//   export mode | Encrypt_input / Encode_plain record the declarations and
//               | op/rotation requirements for ANT
//   run mode    | the same declarations load the ANT material and every
//               | seal::Evaluator operation executes the real clib kernel
//
// The final per-image logits are declared as verifiable outputs by the CNN
// itself (seal::Ciphertext::save_output -> clib::Save_output, injected at
// the staged final decrypt), so `make verify` decrypts them offline and
// checks them with the declared 2.0 tolerance.
//
// Data locations follow the SDK convention (not the CNN's original
// "../../"-relative layout): weights/test images are read under
// CNN_DATA_ROOT (the absolute path of config.mk DATA_DIR, like every app's
// CLIB_APP_DATA_DIR) and the model's own report files are written under
// CNN_RESULT_DIR (this app's result/, created below). Both macros are
// passed by config.mk's build rules.

#include "seal/seal.h"
#include "clib.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

// Model selection (environment, exported by config.mk).
static long g_layers = 20;
static long g_dataset = 10;
static long g_start = 0;
static long g_end = 0;

static const char *env_or(const char *name, const char *fallback)
{
    const char *value = getenv(name);
    return (value != nullptr && value[0] != '\0') ? value : fallback;
}

// The staged CNN graph entry points (cpu-ckks/single-key/cnn/infer_seal.h);
// declared here so this translation unit does not pull the CNN's NTL-using
// headers (NTL lives on the link line only for the staged objects).
void ResNet_cifar10_seal_sparse(size_t layer_num, size_t start_image_id, size_t end_image_id);
void ResNet_cifar100_seal_sparse(size_t layer_num, size_t start_image_id, size_t end_image_id);

// clib application graph: Run_application created the context, so adopt it
// and let the SEAL-compatible layer evaluate the CNN on the real backend.
static void Main_graph(clib::CONTEXT ctx)
{
    seal::adopt_clib_context(ctx);

    cout << "model: ResNet-" << g_layers << endl;
    cout << "dataset: CIFAR-" << g_dataset << endl;
    cout << "start image: " << g_start << endl;
    cout << "end image: " << g_end << endl;

    // The report writes inside the CNN expect the app's result/ to exist.
    error_code ec;
    filesystem::create_directories(CNN_RESULT_DIR, ec);

    if (g_dataset == 10) ResNet_cifar10_seal_sparse(g_layers, g_start, g_end);
    else if (g_dataset == 100) ResNet_cifar100_seal_sparse(g_layers, g_start, g_end);
    else throw invalid_argument("dataset number is not correct");

    seal::release_clib_context();
}

int main(int argc, char **argv)
{
    g_layers = atol(env_or("CNN_LAYERS", "20"));
    g_dataset = atol(env_or("CNN_DATASET", "10"));
    g_start = atol(env_or("CNN_START", "0"));
    g_end = atol(env_or("CNN_END", "0"));

    if (g_start < 0 || g_start >= 10000) throw invalid_argument("start number is not correct");
    if (g_end < 0 || g_end >= 10000) throw invalid_argument("end number is not correct");
    if (g_start > g_end) throw invalid_argument("start number is larger than end number");
    if (g_dataset == 10 || g_dataset == 100) {
        if (g_layers != 20 && g_layers != 32 && g_layers != 44 && g_layers != 56 && g_layers != 110)
            throw invalid_argument("layer number is not valid");
    } else throw invalid_argument("dataset number is not correct");

    return clib::Run_application(argc, argv, Main_graph);
}