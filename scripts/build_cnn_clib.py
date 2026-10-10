#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Build FHE-MP-CNN (cnn_ckks) linked against the clib SDK's SEAL-compatible
# API (baselines/fpga/clib-seal on top of baselines/fpga/clib-sdk).
#
# This mirrors the phantom-fhe GPU pipeline (scripts/ace_compile_gpu.py +
# scripts/build_target_gpu.sh) but for the SEAL/CKKS path. The cnn_ckks
# source tree is never modified in place: all adaptations are applied to a
# disposable staged copy under baselines/fpga/build/, which cmake then uses
# as its source root. Steps:
#
#   1. Install the SEAL-compatible CMake package backed by the clib SDK
#      (scripts/build_clib_sdk.py -> baselines/fpga/build/clib-seal-pkg), so
#      cnn_ckks's find_package(SEAL 3.6) / SEAL::seal resolves to the clib
#      backend instead of the bundled modified SEAL 3.6.6;
#   2. Stage a fresh copy of the cnn_ckks tree into
#      baselines/fpga/build/cnn-ckks-src and adapt it to the clib backend:
#      - give each logical level two half-size primes in infer_seal.cpp
#        (clib::Rescale_ciph / clib::Modswitch consume 2 Q limbs per SEAL
#        level) and trim the chain to 62 physical limbs, clib's maximum
#        (remaining_level 16 -> 15);
#      - bootstrap a second time after each in-loop approx ReLU: the
#        FHE-MP-CNN cycle consumes 16 logical levels between refreshes
#        while a Q=62 clib bootstrap spans only 15 (32 Q limbs, the SDK
#        maximum), so every op must fit in a rescheduled two-bootstrap
#        cycle (values unchanged; only level scheduling differs);
#      - replace the SEAL-internal Bootstrapper with the clib-backed one;
#      - replace run/run_cnn.cpp with the clib driver: with CLIB_MODE in
#        the environment the program runs under clib::Run_application
#        (export/run) and the CNN parameters come from CNN_LAYERS /
#        CNN_DATASET / CNN_START / CNN_END; without it the direct
#        golden-model invocation (./cnn LAYERS DATASET START END) is kept;
#      - declare the final per-image logits as a verifiable output
#        (seal::Ciphertext::save_output) so the ANT verify step decrypts
#        them offline against the golden reference;
#      - re-root the ReLU minimax composition coefficient read
#        (SEALcomp.cpp "../result/d<alpha>.txt") onto CNN_DATA_ROOT and fail
#        hard when the file is missing: upstream's silent-failure mode
#        leaves every coefficient uninitialized and the network quietly
#        degenerates to a ReLU-less classifier (the file itself is tracked
#        at cnn_ckks/result/d13.txt and linked into baselines/fpga/result);
#   3. Re-root the CNN's "../../"-relative data opens (pretrained_parameters /
#      testFile / testFile_cifar100 / result) onto the CNN_DATA_ROOT /
#      CNN_RESULT_DIR macros, so the clib-sdk application port
#      (clib-sdk/apps/fhe-mp-cnn; DATA_DIR-relative like every SDK app) can
#      place them anywhere; the macro defaults reproduce the original
#      "../../" layout bit-for-bit, keeping the direct golden-model and cmake
#      flows unchanged;
#   4. Link the FHE-MP-CNN data files (pretrained_parameters / testFile /
#      testFile_cifar100) at baselines/fpga/, two levels above the run
#      directory, where the macros' defaults expect them (the same files the
#      gpu route ships);
#   5. Configure and build the `cnn` executable in baselines/fpga/build/cnn-ckks.
#
# With --stage-only the script stops after steps 1-4 (no cmake): the staged
# tree is what the clib-sdk app baselines/fpga/clib-sdk/apps/fhe-mp-cnn
# compiles via its own SDK-conventional Makefile (config.mk adds the staged
# sources + clib-seal shim to the application build; `make` there runs the
# full export -> context -> encode -> run -> verify flow). Without it,
# scripts/run_cnn_clib.py drives the real-backend flow over the built
# executable; scripts/perf_cnn_clib.py runs and parses the benchmark.

import os
import re
import shutil
import subprocess
import sys

CWD = '/app'

FHE_ROOT = os.path.join(CWD, 'baselines', 'fpga')
CNN_SRC = os.path.join(FHE_ROOT, 'cnn_ckks')

# Staged (adapted) source root; rebuilt from CNN_SRC on every run, so it
# always starts from a pristine tree and can be deleted at any time.
STAGE_SRC = os.path.join(FHE_ROOT, 'build', 'cnn-ckks-src')
STAGE_INFER_SEAL = os.path.join(STAGE_SRC, 'cpu-ckks', 'single-key', 'cnn', 'infer_seal.cpp')
STAGE_SEALCOMP = os.path.join(STAGE_SRC, 'cpu-ckks', 'single-key', 'comp', 'SEALcomp.cpp')
STAGE_RUN_CNN = os.path.join(STAGE_SRC, 'run', 'run_cnn.cpp')
STAGE_BOOT_DIR = os.path.join(STAGE_SRC, 'cpu-ckks', 'single-key', 'ckks_bootstrapping')

TEMPLATES = os.path.join(FHE_ROOT, 'clib-seal')
BUILD_CLIB_SDK = os.path.join(CWD, 'scripts', 'build_clib_sdk.py')
SEAL_PKG = os.path.join(FHE_ROOT, 'build', 'clib-seal-pkg')
SEAL_PKG_CMAKE = os.path.join(SEAL_PKG, 'lib', 'cmake', 'SEAL-3.6')

BUILD_DIR = os.path.join(FHE_ROOT, 'build', 'cnn-ckks')

# The cnn executable opens "../../pretrained_parameters/...",
# "../../testFile/..." and "../../testFile_cifar100/..." relative to its
# cwd = BUILD_DIR, i.e. at FHE_ROOT, and writes results to FHE_ROOT/result.
# The fpga route carries no local copies of these trees; the gpu route
# ships the identical FHE-MP-CNN data (tracked in git), so link to it
# instead of duplicating ~120 MB.
GPU_ROOT = os.path.join(CWD, 'baselines', 'gpu')
DATA_DIRS = ('pretrained_parameters', 'testFile', 'testFile_cifar100')
RESULT_DIR = os.path.join(FHE_ROOT, 'result')

# ReLU minimax composition coefficients: minimax_ReLU_seal reads them from
# "../result/d<alpha>.txt" relative to its cwd (SEALcomp.cpp). The upstream
# repo ships cnn_ckks/result/d13.txt; if the file is missing, `in >> temp`
# on a failed stream leaves temp UNINITIALIZED and the network silently
# degenerates to a ReLU-less (linear-activation) classifier - deterministic
# logits from multiplied-by-zero-underflow junk, so nothing crashes and the
# damage is invisible unless per-run exports are compared (the clib
# export/compute contract catches it, as nondeterministic declarations).
SEALCOMP_ADDR_ORIG = re.compile(
    r'([ \t]*)string addr = "\.\./result";[ \t]*\n')
SEALCOMP_ADDR_NEW = (
    r'\1// clib-sdk data root (staged by scripts/build_cnn_clib.py): the\n'
    r'\1// ReLU composition coefficients live under the repo data area,\n'
    r'\1// not in a "../result" adjacent to an arbitrary run directory.\n'
    r'\1#ifndef CNN_DATA_ROOT\n'
    r'\1#define CNN_DATA_ROOT "../.."\n'
    r'\1#endif\n'
    r'\1string addr = CNN_DATA_ROOT "/result";\n')

# Fail loudly rather than silently reading uninitialized coefficients when
# the file is missing (see the comment above; the original leaves the
# stream failed and every decomp coefficient uninitialized).
SEALCOMP_OPEN_ORIG = re.compile(
    r'([ \t]*)ifstream in\(str\);\n')
SEALCOMP_OPEN_NEW = (
    r'\1ifstream in(str);\n'
    r'\1// staged hardening: a missing coefficient file must abort the run,\n'
    r'\1// not silently produce uninitialized (junk) decomposition\n'
    r'\1// coefficients - with them the network degenerates to a\n'
    r'\1// ReLU-less classifier while still printing plausible logits.\n'
    r'\1if(!in.is_open()) throw std::runtime_error("ReLU composition coefficient file is not open: " + str);\n')

# Original FHE-MP-CNN coeff_modulus chain: one prime per level. The
# indentation group keeps the rewrite working at both call sites, which
# use different indent styles.
LEVEL_CHAIN_ORIG = re.compile(
    r"([ \t]*)vector<int> coeff_bit_vec;\n"
    r"\1coeff_bit_vec\.push_back\(logq\);\n"
    r"\1for \(int i = 0; i < remaining_level; i\+\+\) coeff_bit_vec\.push_back\(logp\);\n"
    r"\1for \(int i = 0; i < boot_level; i\+\+\) coeff_bit_vec\.push_back\(logq\);\n"
    r"\1coeff_bit_vec\.push_back\(log_special_prime\);")

# Final-result decrypt, shared by the cifar10/cifar100 inference flows: a
# verifiable-output declaration (seal::Ciphertext::save_output) is injected
# in front of it so the ANT verify step decrypts the logits offline.
FINAL_DECRYPT = re.compile(r"([ \t]*)decryptor\.decrypt\(cnn\.cipher\(\), plain\);")

# clib supports an even physical Q of at most 62 limbs (clib.h / SDK docs:
# ANT context Q in 32..62). The adapted chain spends 2 limbs per logical
# level: 2 (input) + 2*remaining_level + 2*boot_level + 2 (special) must be
# <= 62, so remaining_level drops from 16 to 15 (2 + 30 + 28 + 2 = 62).
REMAINING_LEVEL_ORIG = re.compile(r"([ \t]*)int remaining_level = 16;")
REMAINING_LEVEL_NEW = (
    r"\1// clib supports at most 62 physical Q limbs (even): 2 + 2*15 + 2*14 + 2.\n"
    r"\1int remaining_level = 15; // Calculation required")

# In-loop approx-ReLU call sites: the four per-stage ReLU calls inside the
# j/k loops (unlike the layer-0 call, which is indented by two tabs). The
# second bootstrap inserted after them (see insert_cycle_bootstrap) is
# attributed to the enclosing j block, exactly like the main bootstrap.
IN_LOOP_RELU = re.compile(
    r"([ \t]*\t\t\t\t)(approx_ReLU_seal_print\(cnn, cnn, comp_no, deg, alpha, tree, "
    r"scaled_val, logp, encryptor, evaluator, decryptor, encoder, public_key, secret_key, "
    r"relin_keys, B, output, context, gal_keys, stage\);)[ \t]*\n")

# Data-open redirect for the clib-sdk application port: every "../../"-relative
# open in infer_seal.cpp (44 x pretrained_parameters + 4 x testFile/cifar100
# reads, 12 x result writes) is re-rooted onto a compile-time macro. The
# clib-sdk Makefile convention gives apps their data root via
# -DCLIB_APP_DATA_DIR='"<abspath of config.mk DATA_DIR>"' (resnet20 app), so
# the staged CNN uses CNN_DATA_ROOT (weights/test images) and CNN_RESULT_DIR
# (the model's own report files, e.g. per-image logits dumps) in the same
# style. The injected defaults reproduce the original layout exactly
# ("../.." data, "../../result" reports), so the direct golden-model and
# cmake flows keep their behavior bit-for-bit.
DATA_WRITE_PREFIX = re.compile(r'"\.\./\.\./result/')
DATA_READ_PREFIX = re.compile(r'"\.\./\.\./')
INFER_INCLUDE = re.compile(r'(#include "infer_seal\.h"\n)')
# Literals are written substitution-immune ("../.." never matches the
# "../../"-prefix patterns) because the preamble is inserted after the
# redirects - but keep them immune regardless of insertion order.
DATA_ROOT_PREAMBLE = (
    '\n'
    '// clib-sdk app data roots (staged by scripts/build_cnn_clib.py): every\n'
    '// ../../-relative open in this file is re-rooted onto compile-time\n'
    '// macros so the clib-sdk application (clib-sdk/apps/fhe-mp-cnn) can\n'
    '// point them at its own DATA_DIR/result layout. The defaults reproduce\n'
    '// the original repository layout for direct runs of the staged tree.\n'
    '#ifndef CNN_DATA_ROOT\n'
    '#define CNN_DATA_ROOT "../.."\n'
    '#endif\n'
    '#ifndef CNN_RESULT_DIR\n'
    '#define CNN_RESULT_DIR "../.." "/result"\n'
    '#endif\n')

# Driver for the clib application flow (clib::Run_application). With
# CLIB_MODE=export|run in the environment the program runs as a clib
# application: the driver opens the context (Open_context), the graph
# adopts it (seal::adopt_clib_context) so every SEAL-level
# encode/encrypt/evaluate reaches the real clib backend (Encrypt_input /
# Encode_plain / homomorphic kernels), and the driver closes the context at
# the end (Close_context). Without CLIB_MODE the direct golden-model
# invocation (./cnn LAYERS DATASET START END) is kept, unchanged.
RUN_CNN_DRIVER = r'''// FHE-MP-CNN driver (staged copy; written by scripts/build_cnn_clib.py).
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "seal/seal.h"
#include "cnn_seal.h"
#include "infer_seal.h"

using namespace std;

static long g_layers = 20;
static long g_dataset = 10;
static long g_start = 0;
static long g_end = 0;

static const char *env_or(const char *name, const char *fallback)
{
	const char *value = getenv(name);
	return (value != nullptr && value[0] != '\0') ? value : fallback;
}

// clib application graph: the driver opened the context, so adopt it and
// let the SEAL-compatible layer evaluate on the real clib backend.
static void cnn_clib_graph(clib::CONTEXT ctx)
{
	seal::adopt_clib_context(ctx);

	cout << "model: ResNet-" << g_layers << endl;
	cout << "dataset: CIFAR-" << g_dataset << endl;
	cout << "start image: " << g_start << endl;
	cout << "end image: " << g_end << endl;

	if (g_dataset == 10) ResNet_cifar10_seal_sparse(g_layers, g_start, g_end);
	else if (g_dataset == 100) ResNet_cifar100_seal_sparse(g_layers, g_start, g_end);
	else throw std::invalid_argument("dataset number is not correct");

	seal::release_clib_context();
}

static int run_under_clib(const char *mode)
{
	g_layers = atol(env_or("CNN_LAYERS", "20"));
	g_dataset = atol(env_or("CNN_DATASET", "10"));
	g_start = atol(env_or("CNN_START", "0"));
	g_end = atol(env_or("CNN_END", "0"));

	if (g_start < 0 || g_start >= 10000) throw std::invalid_argument("start number is not correct");
	if (g_end < 0 || g_end >= 10000) throw std::invalid_argument("end number is not correct");
	if (g_start > g_end) throw std::invalid_argument("start number is larger than end number");

	// Clib driver arguments (space-separated "--clib-mode export --N ...",
	// as the SDK Makefile passes them); the CNN parameters travel through
	// the environment, not through argv.
	static const string arg_n = env_or("CLIB_N", "65536");
	static const string arg_q = env_or("CLIB_Q", "62");
	static const string arg_work = env_or("CLIB_WORK", "work");
	static const string arg_output = env_or("CLIB_OUTPUT", "result");
	vector<char *> clib_argv;
	clib_argv.push_back(const_cast<char *>("cnn"));
	clib_argv.push_back(const_cast<char *>("--clib-mode"));
	clib_argv.push_back(const_cast<char *>(mode));
	clib_argv.push_back(const_cast<char *>("--N"));
	clib_argv.push_back(const_cast<char *>(arg_n.c_str()));
	clib_argv.push_back(const_cast<char *>("--Q"));
	clib_argv.push_back(const_cast<char *>(arg_q.c_str()));
	clib_argv.push_back(const_cast<char *>("--work"));
	clib_argv.push_back(const_cast<char *>(arg_work.c_str()));
	clib_argv.push_back(const_cast<char *>("--output"));
	clib_argv.push_back(const_cast<char *>(arg_output.c_str()));
	return clib::Run_application(
		static_cast<int>(clib_argv.size()), clib_argv.data(), cnn_clib_graph);
}

int main(int argc, char **argv) {
	if (const char *mode = getenv("CLIB_MODE"))
		return run_under_clib(mode);

	int layer = atoi(argv[1]);
	int dataset = atoi(argv[2]);
	int start = atoi(argv[3]);
	int end = atoi(argv[4]);

	if (start < 0 || start >= 10000) throw std::invalid_argument("start number is not correct");
	if (end < 0 || end >= 10000) throw std::invalid_argument("end number is not correct");
	if (start > end) throw std::invalid_argument("start number is larger than end number");

	cout << "model: ResNet-" << layer << endl;
	cout << "dataset: CIFAR-" << dataset << endl;
	cout << "start image: " << start << endl;
	cout << "end image: " << end << endl;

	if (dataset == 10) ResNet_cifar10_seal_sparse(layer, start, end);
	else if (dataset == 100) ResNet_cifar100_seal_sparse(layer, start, end);
	else throw std::invalid_argument("dataset number is not correct");

	return 0;
}
'''


def run(cmd):
    """Run a command, aborting the build with a clear message if it fails."""
    print(f"\n$ {' '.join(cmd)}")
    ret = subprocess.run(cmd)
    assert ret.returncode == 0, f"command failed: {' '.join(cmd)}"


def install_seal_package():
    """Install the SEAL-compatible package backed by the clib SDK."""
    run([sys.executable, BUILD_CLIB_SDK])


def stage_cnn_src():
    """Copy the pristine cnn_ckks tree into the build area for adaptation."""
    if os.path.lexists(STAGE_SRC):
        shutil.rmtree(STAGE_SRC)
    shutil.copytree(CNN_SRC, STAGE_SRC)


def adapt_level_chain():
    """Give each logical level two half-size primes (2 Q limbs per level)."""

    def repl(m):
        i = m.group(1)
        return (
            i + "// clib's clib::Rescale_ciph / clib::Modswitch consume 2 Q limbs\n"
            + i + "// per SEAL level, so each logical level is represented by two primes\n"
            + i + "// of half the original bit size. A rescale then divides the scale by\n"
            + i + "// the product of the two dropped primes (~2^logp), matching SEAL's\n"
            + i + "// one-prime rescale and keeping the CNN's scale stable, while each\n"
            + i + "// rescale/mod_switch drops exactly one logical level.\n"
            + i + "vector<int> coeff_bit_vec;\n"
            + i + "const int logq_half = logq / 2;\n"
            + i + "const int logp_half = logp / 2;\n"
            + i + "coeff_bit_vec.push_back(logq_half);\n"
            + i + "coeff_bit_vec.push_back(logq_half);\n"
            + i + "for (int i = 0; i < remaining_level; i++) { coeff_bit_vec.push_back(logp_half); coeff_bit_vec.push_back(logp_half); }\n"
            + i + "for (int i = 0; i < boot_level; i++) { coeff_bit_vec.push_back(logq_half); coeff_bit_vec.push_back(logq_half); }\n"
            + i + "coeff_bit_vec.push_back(log_special_prime / 2);\n"
            + i + "coeff_bit_vec.push_back(log_special_prime / 2);")

    with open(STAGE_INFER_SEAL) as f:
        src = f.read()
    src, n = LEVEL_CHAIN_ORIG.subn(repl, src)
    if n == 0:
        raise SystemExit(f"could not find the coeff_bit_vec block in {STAGE_INFER_SEAL}")
    with open(STAGE_INFER_SEAL, 'w') as f:
        f.write(src)
    print(f"infer_seal.cpp level chain adapted ({n} sites)")


def install_bootstrapper():
    """Replace the SEAL-internal Bootstrapper with the clib-backed one."""
    print("install clib-backed Bootstrapper")
    for name in ('Bootstrapper.h', 'Bootstrapper.cpp'):
        shutil.copyfile(os.path.join(TEMPLATES, name),
                        os.path.join(STAGE_BOOT_DIR, name))


def trim_level_budget():
    """Trim the adapted chain from 64 to 62 physical Q limbs (clib max)."""
    with open(STAGE_INFER_SEAL) as f:
        src = f.read()
    src, n = REMAINING_LEVEL_ORIG.subn(REMAINING_LEVEL_NEW, src)
    if n == 0:
        raise SystemExit(f"could not find the remaining_level declaration in {STAGE_INFER_SEAL}")
    with open(STAGE_INFER_SEAL, 'w') as f:
        f.write(src)
    print(f"infer_seal.cpp level budget trimmed to Q62 ({n} sites)")


def inject_save_output():
    """Declare the final logits as verifiable outputs (clib::Save_output)."""
    with open(STAGE_INFER_SEAL) as f:
        src = f.read()

    def repl(m):
        i = m.group(1)
        return (
            i + "// clib backend: declare the final logits as a named output;\n"
            + i + "// the ANT verify step decrypts them offline and checks them\n"
            + i + "// against this golden reference (no-op in golden-model runs).\n"
            + i + 'cnn.cipher().save_output(("fc_logits_" + to_string(image_id)).c_str(), 2.0);\n'
            + i + "decryptor.decrypt(cnn.cipher(), plain);")

    src, n = FINAL_DECRYPT.subn(repl, src)
    if n == 0:
        raise SystemExit(f"could not find the final decrypt in {STAGE_INFER_SEAL}")
    with open(STAGE_INFER_SEAL, 'w') as f:
        f.write(src)
    print(f"infer_seal.cpp save_output injected ({n} sites)")


def insert_cycle_bootstrap():
    """Bootstrap a second time per stage so the cycle fits the clib budget.

    clib's Bootstrap refreshes a Q=62 context to at most Q-30 = 32 limbs,
    i.e. 15 logical levels above the 2-limb bootstrap entry (clib.h).
    The FHE-MP-CNN cycle between consecutive bootstraps needs 16 logical
    levels (approx-ReLU tree 13 + sgn product 1 + multiplexed convolution
    2; batch norm and the residual add are level-free), so the SDK op
    validator rejects the graph at the first convolution whose second
    multiplication round lands below the >=4 Q limbs that Mul_plain
    demands ("application: multiply level/degree mismatch").

    The reschedule: bootstrap again right after each in-loop ReLU, which
    splits every cycle into two legal pockets - tree+product (14) between
    the main and the new bootstrap, conv+BN (2) between the new bootstrap
    and the next main one. The pre-bootstrap mod-switch ladder absorbs
    the unused levels value-exactly, so the CNN's numerics are unchanged;
    only the level scheduling differs from the original single-bootstrap
    cycle reserving level bookkeeping (see SEALContext / Bootstrapper in
    clib-seal).
    """

    def repl(m):
        i = m.group(1)
        return (
            i + m.group(2) + "\n"
            + i + "// clib backend: the Q=62 Bootstrap refreshes to 32 limbs (15 logical\n"
            + i + "// levels), one short of this cycle's 16 (ReLU 14 + convolution 2),\n"
            + i + "// so bootstrap again right after the ReLU: the pockets around the\n"
            + i + "// new bootstrap (tree+product 14, conv+BN 2) both fit. This is a\n"
            + i + "// pure level reschedule - the pre-bootstrap mod-switch ladder is\n"
            + i + "// value-exact, so the CNN's numerics are unchanged.\n"
            + i + "if(j==0) bootstrap_print(cnn, cnn, bootstrapper_1, output, decryptor, encoder, context, stage);\n"
            + i + "else if(j==1) bootstrap_print(cnn, cnn, bootstrapper_2, output, decryptor, encoder, context, stage);\n"
            + i + "else if(j==2) bootstrap_print(cnn, cnn, bootstrapper_3, output, decryptor, encoder, context, stage);\n")

    with open(STAGE_INFER_SEAL) as f:
        src = f.read()
    src, n = IN_LOOP_RELU.subn(repl, src)
    if n != 4:
        raise SystemExit(
            f"expected 4 in-loop approx-ReLU call sites in {STAGE_INFER_SEAL}, found {n}")
    with open(STAGE_INFER_SEAL, 'w') as f:
        f.write(src)
    print(f"infer_seal.cpp per-cycle second bootstrap inserted ({n} sites)")


def adapt_relu_coefficients():
    """Re-root and harden the ReLU composition coefficient read.

    minimax_ReLU_seal reads the decomposition coefficients from
    "../result/d<alpha>.txt" (SEALcomp.cpp). The app Makefile passes
    -DCNN_DATA_ROOT=<abs DATA_DIR> (the same macro infer_seal.cpp uses), and
    the cmake/golden route keeps the original layout via the "../.." default
    (run cwd baselines/fpga/build/cnn-ckks -> baselines/fpga/result, where
    link_data_files links the tracked coefficient file). The staged !in guard
    turns a missing file into a hard error; without it every coefficient
    comes back uninitialized and the network silently loses its ReLU (see
    the SEALCOMP_ADDR_ORIG comment).
    """
    with open(STAGE_SEALCOMP) as f:
        src = f.read()
    src, n_addr = SEALCOMP_ADDR_ORIG.subn(SEALCOMP_ADDR_NEW, src)
    if n_addr != 1:
        raise SystemExit(f"expected 1 coefficient-address line in {STAGE_SEALCOMP}, found {n_addr}")
    src, n_open = SEALCOMP_OPEN_ORIG.subn(SEALCOMP_OPEN_NEW, src)
    if n_open != 1:
        raise SystemExit(f"expected 1 coefficient-file open in {STAGE_SEALCOMP}, found {n_open}")
    with open(STAGE_SEALCOMP, 'w') as f:
        f.write(src)
    print(f"SEALcomp.cpp coefficient read re-rooted + hardened ({n_addr}+{n_open} sites)")


def write_run_cnn_driver():
    """Install the clib application driver as run/run_cnn.cpp."""
    with open(STAGE_RUN_CNN, 'w') as f:
        f.write(RUN_CNN_DRIVER + '\n')
    print("run_cnn.cpp clib driver installed")


def redirect_data_opens():
    """Re-root the CNN's "../../"-relative opens onto CNN_DATA_ROOT/result.

    The clib-sdk application convention places an app's raw data at the
    absolute CLIB_APP_DATA_DIR (config.mk DATA_DIR) and its own report files
    under the app's result/ directory, while the staged tree's original
    opens assume a "../../"-shaped repository layout. Rewriting the opens to
    string-literal-concatenation with CNN_DATA_ROOT / CNN_RESULT_DIR lets
    each build pass its roots on the compile line; the injected macro
    defaults reproduce the original layout exactly, so nothing changes for
    the direct golden-model and cmake flows.
    """

    with open(STAGE_INFER_SEAL) as f:
        src = f.read()

    # Redirect the opens first, then insert the macro preamble: the
    # substitution patterns would otherwise rewrite the preamble's own
    # default literals (comment text included) as if they were data paths.
    src, n_write = DATA_WRITE_PREFIX.subn(r'CNN_RESULT_DIR "/', src)
    src, n_read = DATA_READ_PREFIX.subn(r'CNN_DATA_ROOT "/', src)
    if '"../../' in src:
        raise SystemExit(f"unredirected \"../../\"-relative open left in {STAGE_INFER_SEAL}")
    src, n_inc = INFER_INCLUDE.subn(lambda m: m.group(1) + DATA_ROOT_PREAMBLE, src)
    if n_inc != 1:
        raise SystemExit(f"could not find the infer_seal.h include in {STAGE_INFER_SEAL}")

    with open(STAGE_INFER_SEAL, 'w') as f:
        f.write(src)
    print(f"infer_seal.cpp data opens re-rooted "
          f"({n_read} reads -> CNN_DATA_ROOT, {n_write} writes -> CNN_RESULT_DIR)")


def link_data_files():
    """Expose the FHE-MP-CNN data trees at FHE_ROOT (cnn's "../../" root).

    Without these the cnn run aborts with SIGABRT right after keygen: the
    pretrained_parameters opens throw std::runtime_error("file is not open"),
    while the testFile opens fail silently and poison the inference.
    """
    for name in DATA_DIRS:
        dst = os.path.join(FHE_ROOT, name)
        if os.path.lexists(dst):
            continue
        src = os.path.join(GPU_ROOT, name)
        if not os.path.isdir(src):
            raise SystemExit(f"expected data directory not found: {src}")
        os.symlink(os.path.relpath(src, FHE_ROOT), dst)
        print(f"linked {dst} -> {src}")
    os.makedirs(RESULT_DIR, exist_ok=True)
    # The ReLU minimax composition coefficients (upstream cnn_ckks/result/
    # d13.txt, tracked in the pristine tree; see adapt_relu_coefficients)
    # must also exist at the "../.."/result location the cnn binary reads.
    coeff_dst = os.path.join(RESULT_DIR, 'd13.txt')
    coeff_src = os.path.join(CNN_SRC, 'result', 'd13.txt')
    if not os.path.isfile(coeff_src):
        raise SystemExit(
            f"ReLU composition coefficient file not found: {coeff_src}\n"
            f"(upstream FHE-MP-CNN cnn_ckks/result/d13.txt; without it the CNN "
            f"silently degenerates to a ReLU-less classifier)")
    if not os.path.lexists(coeff_dst):
        os.symlink(os.path.relpath(coeff_src, RESULT_DIR), coeff_dst)
        print(f"linked {coeff_dst} -> {coeff_src}")


def reset_stale_build_dir():
    """Drop a build dir that was configured for a different source root.

    cmake refuses to reconfigure a cached build dir against another -S
    (e.g. one left by an earlier run against CNN_SRC, before the source
    staging was introduced). Wipe such dirs; reconfiguration then rebuilds
    against STAGE_SRC, reusing nothing.
    """
    cache = os.path.join(BUILD_DIR, 'CMakeCache.txt')
    if not os.path.isfile(cache):
        return
    cached_src = None
    with open(cache) as f:
        for line in f:
            if line.startswith('CMAKE_HOME_DIRECTORY:INTERNAL='):
                cached_src = line.split('=', 1)[1].strip()
                break
    if cached_src != os.path.realpath(STAGE_SRC):
        print(f"reset stale build dir (was configured for {cached_src})")
        shutil.rmtree(BUILD_DIR)


def build_cnn():
    """Configure and build the cnn executable against the clib SDK package."""
    reset_stale_build_dir()
    run(['cmake', '-S', STAGE_SRC, '-B', BUILD_DIR,
         '-DCNN=ON', '-DCMAKE_BUILD_TYPE=Release',
         f'-DSEAL_DIR={SEAL_PKG_CMAKE}'])
    run(['cmake', '--build', BUILD_DIR, '--', '-j'])

    exe = os.path.join(BUILD_DIR, 'cnn')
    if not os.path.exists(exe):
        raise SystemExit(f"expected executable not found: {exe}")
    print(f"\nFHE-MP-CNN built: {exe}")


def main():
    install_seal_package()
    stage_cnn_src()
    adapt_level_chain()
    trim_level_budget()
    inject_save_output()
    insert_cycle_bootstrap()
    redirect_data_opens()
    adapt_relu_coefficients()
    install_bootstrapper()
    write_run_cnn_driver()
    link_data_files()
    if '--stage-only' in sys.argv:
        print(f"\nstaged copy ready (no cmake build): {STAGE_SRC}\n"
              f"the clib-sdk app clib-sdk/apps/fhe-mp-cnn compiles it via its Makefile")
        return
    build_cnn()


if __name__ == '__main__':
    main()