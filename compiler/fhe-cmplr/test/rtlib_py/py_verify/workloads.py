"""Workload table for the Phase 1 execution gate (e2e + allops) AND the
rtlib-py-onnx real-ONNX integration plan (doc/_archive/rtlib-py-onnx/PLAN.md).

Each workload pairs an ONNX source with the plaintext inputs/expected the
rtlib_py.<rt> executor checks against. `run_py_verify` drives: build/copy onnx
-> `fhe_cmplr -CKKS2PY:lib=openfhe` -> `py_compile` -> `run_py_exec.run(py, spec)`
-> PASS/FAIL/XFAIL.

Two workload families:

(1) The original synthetic green gate (no `onnx_file`): add1 + mul1 + mulc2 +
    mulc2df (add8 adds slot-vector coverage). mulc2 was an opt-in XFAIL through
    Phase 2.1; Phase 2.2 closed the array-constant -> Python-list emitter gap
    and the raw_data constant form, so it joins the default green gate; Phase
    2.3 adds mulc2df, the same model compiled with `-CKKS2PY:df=` so the 2.5 weight
    is encoded offline from the C binary side data file via
    rt_ref.load_data_entry:

      add1    A[1]+B[1] -> Y       ciph+ciph add_ciph, 1 slot, mul_depth 0
      mul1    A[1]*B[1] -> Y       mul_ciph -> rescale_ciph -> relin, mul_depth 1
      add8    A[8]+B[8] -> Y       8-slot elementwise add_ciph (slot-vector)
      mulc2   A[1]*2.5  -> Y       constant-folded encode_float(_cst_0)+mul_plain+
                                  rescale; 2.0*2.5 = 5.0 under rt_ref
      mulc2df A[1]*2.5  -> Y       same as mulc2 but `-CKKS2PY:df=`: the 2.5 weight is
                                  serialized to the C binary data file and read
                                  back via load_data_entry (offline encode, 2.3)

(2) The rtlib-py-onnx plan workloads (`onnx_file=<abs path>` -> copy the real
    onnx-models/ model verbatim, instead of `build`ing a synthetic one). They
    span the plan's 4 empirical integration buckets (PLAN.md section B):
      (a) green   -- emit + exec + max_abs=0      (P0 single-op: add/mult/...)
      (b) emitter unrolling bug -- exec NameError ..._block_*  (conv/gemm/mvp/...)
      (c) context-level budget mismatch -- exec [hard] ... level out of bounds
                                                  (relu/pool/gap/reshape)
      (d) numerical error -- exec ok but max_abs>tol (none observed this round)
    Bucket (b)/(c) workloads are opt-in XFAIL with the TRUE cause in
    `xfail_reason` (PLAN.md section G.3/G.4); heavy full-nets (resnet110/AlexNet) are
    `compile_only` (compile + py_compile, no exec); the P4 synthetic probes
    (mulmul/concat) are `probe` (excluded from the default/--all green gate,
    run via --workload/--include-probes).

`spec_from_workload` produces the dict `run_py_exec.run` consumes (inputs/
expected keyed by the ONNX graph tensor names the emitter binds).
"""

import os

import gen_workload_onnx as G

_ONNX_DIR = None  # set by run_py_verify to a per-run temp dir

# Root of the real-ONNX corpus for the rtlib-py-onnx plan workloads (onnx_file
# workloads copy a model from here verbatim instead of synthesizing one).
_ONNX_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "..", "..", "onnx-models"))


def _onnx(rel):
    """Absolute path to a corpus model under onnx-models/ (for onnx_file=)."""
    return os.path.join(_ONNX_ROOT, rel)


def _pat(n):
    """Deterministic small-magnitude input fill for peripheral (bucket b/c)
    workloads. Values stay in ~[-0.5, 0.5] so they never blow up the rt_openfhe
    toy-context integer-bit budget if a model ever reaches comparison; bucket
    b/c fail before comparison, so the exact values do not affect the bucket
    (they only need to be the right length so the seeded cipher has N slots)."""
    return [round(((i * 7 + 3) % 11 - 5) * 0.1, 2) for i in range(n)]


def _zeros(n):
    """Non-empty, correct-length placeholder expected for peripheral bucket
    b/c workloads. They fail at Main_graph before output comparison, but a
    non-empty `expected` is REQUIRED (driver.py refuses a vacuous empty-expected
    pass) and a correct length keeps the bucket-(d) detection path and the
    future-fixed PASS path honest (PLAN.md section G.3)."""
    return [0.0] * n


class Workload:
    def __init__(self, name, build=None, n=None, inputs=None, expected=None,
                 output_name=None, input_names=None, strict=True, xfail=False,
                 xfail_reason="", df=False, onnx_file=None, prec=None,
                 compile_only=False, heavy=False, probe=False,
                 compile_timeout=None, extra_flags=None, golden_onnx=False):
        self.name = name
        self.build = build          # gen_workload_onnx builder (synthetic path)
        self.n = n                  # tensor length (synthetic path)
        self.inputs = inputs        # {name: [floats]} (graph input names)
        self.expected = expected    # {name: [floats]} (graph output names)
        self.output_name = output_name
        self.input_names = input_names
        self.strict = strict
        self.xfail = xfail
        self.xfail_reason = xfail_reason
        self.df = df                # compile with -CKKS2PY:df=<weights> (offline)
        self.onnx_file = onnx_file  # PLAN: copy this real onnx verbatim
        self.prec = prec            # PLAN G.2: per-workload epsilon override
        self.compile_only = compile_only  # PLAN: compile+py_compile only (heavy)
        self.heavy = heavy          # PLAN: excluded from default/--all; OOM guard
        self.probe = probe          # PLAN P4: excluded from default/--all gate
        self.compile_timeout = compile_timeout  # PLAN: per-workload compile wall cap
        self.extra_flags = extra_flags  # PLAN P4: extra fhe_cmplr args (raised ctx)
        # golden_onnx: compute `expected` at runtime via onnxruntime (row-major
        # flatten) instead of the static `expected` placeholder. For workloads
        # whose FHE output packing is row-major leading (depthwise conv, gemm) --
        # verified to match; lets the gate compare against the TRUE onnx golden
        # (replaces the _zeros placeholder). Needs onnxruntime (skip if absent).
        self.golden_onnx = golden_onnx

    def onnx_path(self, onnx_dir):
        return os.path.join(onnx_dir, f"{self.name}.onnx")

    def write_onnx(self, onnx_dir):
        path = self.onnx_path(onnx_dir)
        if getattr(self, "onnx_file", None):
            # PLAN G.1: copy the real corpus model verbatim (no synthesis, so
            # the test exercises the actual onnx-models/ network the plan
            # targets -- opset/ir/weights/lowering identical to a real run).
            import shutil
            shutil.copyfile(self.onnx_file, path)
        else:
            self.build(path, self.n)
        return path


def _arith(n, a, b):
    return [a + b for a, b in zip(a, b)]


# True-cause strings for the workloads' current failure cause. Each
# b/c-peripheral workload fails at the second layer -- one of the defects below.
# The classifier (run_py_verify._classify_xfail) matches the ACTUAL runtime
# detail; these documentary reasons record the root cause (the C ant runtime
# tolerates/auto-handles these, which is why the same lowering passes on the C
# backend -- these are Python-side gaps).
_DEEP_BCAST = ("second-layer [hard] mul_plain: plain/cipher slot-count broadcast "
               "mismatch -- the Python plain-packing in the Conv/Gemm/Matvec "
               "emission gives a plain whose slot count != the cipher's and is "
               "neither scalar nor full-length, so rt_ref/rt_openfhe refuse the "
               "broadcast (e.g. 6144 vs 3072); exposed once the b/c fix lets exec "
               "reach the first mul_plain")
_DEEP_LEVEL = ("second-layer [strict] add_ciph/sub_ciph level mismatch -- gap #1/#2 "
               "deep-context level management: the relu/pool polynomial tree adds "
               "ciphers at different levels with no mod_switch alignment; exposed "
               "once the b/c fix lets exec reach the tree's add")
_DEEP_NUM = ("second-layer bucket (d) numerical: executes to completion after the "
             "b/c fix but the output != ONNX golden. Verified via onnxruntime: the "
             "output multiset differs from the true ONNX conv golden under EVERY "
             "slot layout (so it is NOT a packing permutation) -- a real Python-"
             "side plain-packing defect in the grouped/depthwise-conv emission, "
             "same family as _DEEP_BCAST (it merely didn't trip the broadcast guard)")
# Real VALUE defects, distinct from the (1)/(2) packing/broadcast/level gaps --
# these are accuracy defects in the multi-channel conv / max-pool lowering path
# (depthwise conv & gemm & avg/global-pool & relu & matvec are all CORRECT; see
# golden_onnx workloads). C passes the same lowering on the external platform,
# so these are Python emitter/rt_ref divergences to chase.
_DEEP_CONV = ("real multi-channel-conv cross-channel accumulation defect: the output's "
              "non-zero multiset != the ONNX golden (large values match, small ones "
              "~0.1 off = a dropped/mis-added channel term in the cross-channel sum). "
              "Depthwise/grouped conv (conv_p1_s1_g32) and gemm are CORRECT under the "
              "same (1)(2)+B3 fixes, so the gap is the multi-channel (3+ -> N) sum, "
              "NOT (1)/(2). Same lowering as C (which passes) -> Python-side; needs a "
              "dedicated investigation (not a golden swap).")
_DEEP_MAXPOOL = ("real max-pool lowering defect: rt_ref output wildly differs from the "
                 "ONNX exact-max (the true maxes 0.5/0.3/0.4 are not even present in "
                 "the output; rt_ref gives ~0.175). FHE max-pool is a polynomial "
                 "approx of max -- but the gap is far beyond approximation error for "
                 "inputs in [-0.5,0.5], so it is a lowering/emitter defect (or needs "
                 "a C-side approx reference to bound). NOT (1)/(2); not a golden swap.")

WORKLOADS = {
    # (1) synthetic green gate (add1/mul1/add8/mulc2/mulc2df) retired in favor
    #     of the onnx sweep (the main test path).

    # ---- (2) rtlib-py-onnx plan: P0 single-op green (bucket a) -------------
    # Golden hand-computed and confirmed under rt_ref (max_abs=0). Constants
    # read from the model's Constant node (k=[1,1,1,1], F32-exact), so golden
    # uses the F32-upcast value (PLAN.md section D warning).
    "add": Workload(
        name="add", onnx_file=_onnx("op/add.onnx"),
        inputs={"input": [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0],
                "onnx::Add_1": [9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0]},
        expected={"output": [10.0] * 9},
        output_name="output", input_names=["input", "onnx::Add_1"],
        strict=True, prec=1e-9),
    "mult": Workload(
        name="mult", onnx_file=_onnx("op/mult.onnx"),
        inputs={"input": [1.0, 2.0, 3.0, 4.0],
                "onnx::Mul_1": [2.0, 3.0, 4.0, 5.0]},
        expected={"output": [2.0, 6.0, 12.0, 20.0]},
        output_name="output", input_names=["input", "onnx::Mul_1"],
        strict=True, prec=1e-8),  # depth-1 mul_ciph; rt_openfhe max_abs ~1.6e-9
                          # at magnitude-20 slots (relative ~8e-11) -> section E rule
                          # tol=1e-9*10^1=1e-8 (rt_ref still max_abs=0).
    "add_const": Workload(
        name="add_const", onnx_file=_onnx("op/add_const.onnx"),
        inputs={"input": [1.0, 2.0, 3.0, 4.0]},
        expected={"output": [2.0, 3.0, 4.0, 5.0]},  # + [1,1,1,1]
        output_name="output", input_names=["input"], strict=True, prec=1e-9),
    "mult_const": Workload(
        name="mult_const", onnx_file=_onnx("op/mult_const.onnx"),
        inputs={"input": [2.0, 3.0, 4.0, 5.0]},
        expected={"output": [2.0, 3.0, 4.0, 5.0]},  # * [1,1,1,1] (identity)
        output_name="output", input_names=["input"], strict=True, prec=1e-9),
    "mult_const_df": Workload(
        name="mult_const_df", onnx_file=_onnx("op/mult_const.onnx"), df=True,
        inputs={"input": [2.0, 3.0, 4.0, 5.0]},
        expected={"output": [2.0, 3.0, 4.0, 5.0]},  # gap #6: offline -CKKS2PY:df=
                          # encode of the [1,1,1,1] constant (F32 ARRAY), read
                          # back via load_data_entry, mirroring mulc2df.
        output_name="output", input_names=["input"], strict=True, prec=1e-9),
    "add_1_dimension": Workload(
        name="add_1_dimension", onnx_file=_onnx("op/add_1_dimension.onnx"),
        inputs={"input": [1.0, 2.0, 3.0], "onnx::Add_1": [10.0, 20.0, 30.0]},
        expected={"output": [11.0, 22.0, 33.0]},
        output_name="output", input_names=["input", "onnx::Add_1"],
        strict=True, prec=1e-9),
    "flatten": Workload(
        name="flatten", onnx_file=_onnx("op/flatten.onnx"),
        inputs={"input": [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]},
        expected={"output": [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]},
        output_name="output", input_names=["input"], strict=True, prec=1e-9),
    "base_add_const": Workload(
        name="base_add_const", onnx_file=_onnx("base/add_const.onnx"),
        inputs={"input": [1.0, 2.0, 3.0, 4.0]},  # parity anchor (base/==op/ md5)
        expected={"output": [2.0, 3.0, 4.0, 5.0]},
        output_name="output", input_names=["input"], strict=True, prec=1e-9),

    # ---- P1 (were bucket (b) emitter-bug; b/c FIXED -> now second-layer) ----
    "gemm_2_2x2": Workload(
        name="gemm_2_2x2", onnx_file=_onnx("gemm/gemm_2_2x2.onnx"),
        inputs={"input": _pat(2)}, expected={"output": _zeros(2)},
        output_name="output", input_names=["input"], strict=True, prec=1e-6,
        golden_onnx=True),  # row-major leading; verified rt_ref ~2.4e-8 vs onnxruntime
    "conv_p0_s1": Workload(
        name="conv_p0_s1", onnx_file=_onnx("conv/conv_p0_s1.onnx"),
        inputs={"input": _pat(3072)}, expected={"output": _zeros(4704)},
        output_name="output", input_names=["input"], strict=True,
        xfail=True, xfail_reason=_DEEP_CONV),
    "conv_p1_s1_g32": Workload(
        name="conv_p1_s1_g32", onnx_file=_onnx("cnn/conv_p1_s1_g32.onnx"),
        inputs={"input": _pat(32768)}, expected={"output": _zeros(32768)},
        output_name="output", input_names=["input"], strict=True, prec=1e-6,
        golden_onnx=True),  # depthwise, row-major leading; verified ~5.2e-8 vs onnxruntime
    "mvp_8_4_mvm": Workload(
        name="mvp_8_4_mvm", onnx_file=_onnx("mvp/8_4_mvm.onnx"),
        inputs={"input": _pat(8)}, expected={"output": _zeros(4)},
        output_name="output", input_names=["input"], strict=True, prec=1e-6,
        golden_onnx=True),  # matvec, row-major leading; verified rt_ref max_abs=0
    "gemv_512x512": Workload(
        name="gemv_512x512", onnx_file=_onnx("cnn/gemv_512x512.onnx"),
        inputs={"input": _pat(512)}, expected={"output": _zeros(512)},
        output_name="output", input_names=["input"], strict=True,
        xfail=True, xfail_reason=("frontend lowering limitation: "
                                  "tensor2vector_util k_pad size constraint "
                                  "(TODO) -- compile rc=1, distinct from b/c")),
    "resnet_block1": Workload(
        name="resnet_block1", onnx_file=_onnx("cnn/resnet_block1.onnx"),
        inputs={"input": _pat(16384)}, expected={"output": _zeros(16384)},
        output_name="output", input_names=["input"], strict=True,
        xfail=True, xfail_reason=_DEEP_CONV),  # contains multi-channel conv -> _DEEP_CONV

    # ---- P1 (were bucket (c) context-budget; b/c FIXED -> now second-layer) --
    "relu": Workload(
        name="relu", onnx_file=_onnx("op/relu.onnx"),
        inputs={"input": _pat(9)}, expected={"output": _zeros(9)},
        output_name="output", input_names=["input"], strict=True, prec=1e-6,
        golden_onnx=True),  # relu poly-approx exact for these inputs; row-major; max_abs=0
    "max_pool": Workload(
        name="max_pool", onnx_file=_onnx("op/max_pool.onnx"),
        inputs={"input": _pat(16)}, expected={"output": _zeros(4)},
        output_name="output", input_names=["input"], strict=True,
        xfail=True, xfail_reason=_DEEP_MAXPOOL),
    "gap_hw8": Workload(
        name="gap_hw8", onnx_file=_onnx("avg_pool/gap_hw8.onnx"),
        inputs={"input": _pat(256)}, expected={"output": _zeros(4)},
        output_name="output", input_names=["input"], strict=True, prec=1e-6,
        golden_onnx=True),  # global-avg-pool exact; row-major leading; max_abs=0
    "avg_pool_s2_h4": Workload(
        name="avg_pool_s2_h4", onnx_file=_onnx("avg_pool/avg_pool_s2_h4.onnx"),
        inputs={"input": _pat(48)}, expected={"output": _zeros(12)},
        output_name="output", input_names=["input"], strict=True, prec=1e-6,
        golden_onnx=True),  # avg-pool exact; row-major leading; max_abs=0
    "avg_pool_s2_h10": Workload(
        name="avg_pool_s2_h10", onnx_file=_onnx("cnn/avg_pool_s2_h10.onnx"),
        inputs={"input": _pat(1600)}, expected={"output": _zeros(400)},
        output_name="output", input_names=["input"], strict=True, prec=1e-6,
        golden_onnx=True),  # avg-pool exact; row-major leading; max_abs=0
    "resnet_reshape": Workload(
        name="resnet_reshape", onnx_file=_onnx("cnn/resnet_reshape.onnx"),
        # Phase 0 finding: Reshape [1,64,1,1]->[1,64] lowers to copy_ciph
        # IDENTITY (emitted `output_1 = copy_ciph(input_0)`), so it is bucket
        # (a) GREEN (max_abs=0 under rt_ref), not bucket (c) -- expected == input.
        # rt_openfhe: 64-slot identity sits at the toy-context encrypt/decrypt
        # noise floor (~1.4e-9) -> section E empirical tol = max_abs*10 ~= 1e-8.
        inputs={"input": _pat(64)}, expected={"output": _pat(64)},
        output_name="output", input_names=["input"], strict=True, prec=1e-8),

    # ---- P2 full networks (heavy compile-only; light ones exec-attempt) -----
    # The medium/large full-nets emit 100s-of-MB to ~1 GB .py (resnet20 emitted
    # 941 MB) -- their exec (and even py_compile) is infeasible (Python
    # scalability), so they are compile_only: compile + .c/.conf gate proves the
    # emit path handles full nets; .py size + py_compile-skip recorded. The
    # lightest nets (lenet/lenet_no_relu, ~20 MB .py) exec-attempt and XFAIL at
    # the second layer (mul_plain broadcast; were bucket (b), now fixed past it).
    "resnet20": Workload(
        name="resnet20", onnx_file=_onnx("ace/resnet20_cifar10_pre.onnx"),
        inputs={"input": _pat(3072)}, expected={"output": _zeros(10)},
        output_name="output", input_names=["input"], strict=True,
        compile_only=True, heavy=True, compile_timeout=900),
    "resnet110": Workload(
        name="resnet110", onnx_file=_onnx("ace/resnet110_cifar10_train.onnx"),
        inputs={"input": _pat(3072)}, expected={"output": _zeros(10)},
        output_name="output", input_names=["input"], strict=True,
        compile_only=True, heavy=True, compile_timeout=400),
    "mobilenet": Workload(
        name="mobilenet", onnx_file=_onnx("resbm/MobileNet.onnx"),
        inputs={"input": _pat(3072)}, expected={"output": _zeros(10)},
        output_name="output", input_names=["input"], strict=True,
        compile_only=True, heavy=True, compile_timeout=150),
    "squeezenet": Workload(
        name="squeezenet", onnx_file=_onnx("alinm/squeezenet_cifar10.onnx"),
        inputs={"input": _pat(3072)}, expected={"output": _zeros(10)},
        output_name="output", input_names=["input"], strict=True,
        compile_only=True, heavy=True, compile_timeout=150),
    "lenet_no_relu": Workload(
        name="lenet_no_relu", onnx_file=_onnx("cnn/lenet_no_relu.onnx"),
        inputs={"input": _pat(3072)}, expected={"output": _zeros(10)},
        output_name="output", input_names=["input"], strict=True,
        xfail=True, xfail_reason=_DEEP_CONV),  # contains multi-channel conv -> _DEEP_CONV
    "lenet": Workload(
        name="lenet", onnx_file=_onnx("cnn/lenet.onnx"),
        inputs={"input": _pat(3072)}, expected={"output": _zeros(10)},
        output_name="output", input_names=["input"], strict=True,
        xfail=True, xfail_reason=_DEEP_CONV),  # contains multi-channel conv -> _DEEP_CONV
    "alexnet": Workload(
        name="alexnet", onnx_file=_onnx("resbm/AlexNet.onnx"),
        inputs={"input": _pat(3072)}, expected={"output": _zeros(10)},
        output_name="output", input_names=["input"], strict=True,
        compile_only=True, heavy=True, compile_timeout=150),

    # ---- P4 synthetic probes (excluded from the default/--all green gate;
    #      run via --workload or --include-probes) ---------------------------
    "mulmul": Workload(
        name="mulmul", build=G.build_mul_mul, n=2,
        inputs={"A": [1.0, 2.0], "B": [2.0, 3.0], "C": [1.0, 1.0]},
        expected={"Y": [2.0, 6.0]},  # A*B*C = [1*2*1, 2*3*1] = [2, 6]
        output_name="Y", input_names=["A", "B", "C"], strict=True, prec=1e-7,
        probe=True,  # gap #1/#2 deep-context level mismatch CLOSED by the (2)
                     # level auto-align fix (rt_ref XPASS max_abs=0); kept a probe
                     # (out of the --all gate) per the original P4a framing.
        extra_flags=["-CKKS:icl=3", "-CKKS:mcl=5", "-CKKS:sf=40"]),
    "concat": Workload(
        name="concat", build=G.build_concat, n=2,
        # 4-D (NCHW) inputs A[1,2,1,1], B[1,2,1,1], Concat axis=1 (channel) ->
        # Y[1,4,1,1]; row-major flatten -> [a0,a1,b0,b1] = [1,2,3,4].
        inputs={"A": [1.0, 2.0], "B": [3.0, 4.0]},
        expected={"Y": [1.0, 2.0, 3.0, 4.0]},
        output_name="Y", input_names=["A", "B"], strict=True,
        probe=True,  # the "Y=A+rotate(B,-2) adds-not-concatenates" gap CLOSED
                     # by the (1)-B3 ring-padding fix (rotate now non-identity on
                     # the padded cipher); rt_ref XPASS max_abs=0. kept a probe.
    ),
}


# A hand-built spec for the allops C++ test (its IR loads one cipher "input"
# and stores one cipher "output"). Kept here so run_py_exec's `@<name>` CLI
# form and the allops test share one spec format.
ALLOPS_SPEC = {
    "inputs": {"input": [2.0, 2.0]},
    "expected": {"output": [4.0, 4.0]},  # mul_ciph(e,e), e = input (via the
    #                                        additive gap-op chain), so out = v*v
    "strict": True,
}


def spec_from_workload(wl):
    return {
        "inputs": dict(wl.inputs),
        "expected": dict(wl.expected),
        "strict": wl.strict,
    }
