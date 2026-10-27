# Open device-cmodel issues for the fhert team

Audience: fhert maintainers. This document is self-contained: it does not assume
familiarity with the fhe-cmplr repository or with prior conversations. It lists
device-cmodel problems we cannot fix from our side (the fhert repository is
read-only for us) and gives a reproduction for each. Our completed fix and
verified results are in `RESULTS.md` (same directory).

## 0. Context

We drive the fhert CModelDevice through a small provider module (`rt_fhert`) in
the fhe-cmplr tree. `rt_fhert` builds a CKKS CryptoContext with fhert's
`GenCryptoContext`, uploads it to a `CModelDevice` via `SetParamsTo` /
`SetEvalKeysTo` / `SetBootstrapPrecomTo`, and then calls the standard
`cc.Eval*` / `cc.EvalBootstrap` operations, which your device-patch layer routes
to the cmodel kernels. So the failures below are all inside the fhert device
code path (`fhert/openfhe/_ckks_context.py`, `fhert/backends/cmodel_device.py`,
`fhert/runtime/jit.py`, the cmodel `kernel.bin` subprocess), not in our
provider.

Device config used throughout: ring 65536, grid `MPUH1Config(256, 256, 1, 8)`
(peg = 8, the value your `tests/test_rotate.py` / `examples/profile_bootstrap.py`
use), full packing (32768 slots, i.e. `ring_dim // 2`). Interpreter is CPython
3.12; the fhert `.so` is the cp312 build.

Prerequisite (cmodel version): the cmodel must contain `PerfSnapshot::lines` /
`PerfLineBucket` in `intrinsic/include/fhe_perf.h`, because the latest
`fhert/backends/cmodel_device.py` kernel template (commit 5471dd5) emits
`perf.lines` / `PerfLineBucket`. The `f817c6f3` cmodel (branch
`fhert/HPU-V2-cmodel-v2.3-release`) has it; older cmodels do not, and every
device kernel compilation then fails with
`'struct PerfSnapshot' has no member named 'lines'`. Please ensure the pinned
cmodel and `cmodel_device.py` are version-consistent.

Runtime cache note: the cmodel device lib is built into an `XDG_CACHE_HOME`
salt directory. We redirect it with `XDG_CACHE_HOME=/tmp/fhert_cache_dev` so the
lib is built from the f817c6f3 source; the default `~/.cache` path may hold a
stale lib. This is only a cache-location note, not an issue.

## Issue 1: resnet20 (depth qnum=30, full packing) exceeds the device VRF and HBM capacity on the 256x256 cmodel

### Background: device CRAM ceiling and the `mxbl` lever

The cmodel device constant RAM is the fixed C array
`dsa_bconv_table[PEG_NUM=8][16384]` (1 MiB; 8 PEGs x 16384 entries), holding the
8 base-conversion CRT regions assembled by `assemble_bconv_table`
(`fhert/openfhe/_rns_helpers.py`) and written through `dev.upload_bconv_table`
(`fhert/_C/cmodel.cpp`), which the device path calls at `_ckks_context.py:934`.
Regions 4 (`qhat_inv`) and 5 (`qlInvModqi`) each carry an explicit `(qnum-1)^2`
term and the final block adds another, with Region 7 (`QHatModPi`) adding
further offset growth, so the high-water offset grows roughly quadratically in
qnum. The current cmodel's `upload_bconv_table` checks `data.size() > 16384`
and raises `std::overflow_error`; an earlier investigation observed
the now-retired former `set_params` (which `upload_bconv_table` replaces; see
the cmodel.cpp comments) core-dump at qnum >= 34 on the 256x256 cmodel (exit
139) instead of raising, so the core-dump is historical / version-dependent, not
the current behavior. The threshold is qnum-driven, not slot/packing-driven:
CRT-qnum <= 32 fits (cram_total ~15505 at qnum=32) and qnum >= 34 is over
(qnum=33 predicted over by the offset formula). fhe-cmplr mirrors this offset
math in `rt_fhert._device_bconv_cram_total` and fail-fasts before the ~8-min
keygen + upload with a clear message; the `cram_total` it prints is the same
number used below, and it varies with `dnum` (Region 6 = qnum * dnum * alpha).

`mxbl` is the compiler option `-CKKS:mxbl` (`max_bts_lvl`), the ReSBM
per-segment consumable budget. `ckks/src/resbm.cxx` feeds it to the scheme
search (`Cal_min_laten_plan`): lowering `mxbl` re-plans the scheme with shorter
segments and lower bootstrap landing targets, which lowers the emitted
`mul_depth` and hence CRT-qnum. Reducing the RUNTIME chain (`levels_after` /
`ir_capacity`) without recompiling is the wrong lever (the compiler still emits
the longer plan, so the runtime chain is too shallow and rescale underflows).
For ResNet20: `mxbl=16` (default) gives CRT-qnum 38 under FLEXIBLEAUTOEXT (37
under FIXEDMANUAL); either exceeds the CRAM ceiling, so device setup never
starts. `mxbl=8` gives CRT-qnum 30 under FIXEDMANUAL (31 under FLEXIBLEAUTOEXT;
FIXEDMANUAL has no EXT tower, so qnum = multiplicative_depth + 1), cram_total
~12.3k-12.9k < 16384 (varies with `dnum`; 12902 at the default dnum=3), so CRAM
passes. The host-correct result (pred = 6, qualitatively close to onnxruntime) was
verified under FLEXIBLEAUTOEXT (CRT-qnum 31, cram_total under the 16384 ceiling); the device path
here uses FIXEDMANUAL (qnum 30), whose host-correctness we did not separately
verify (it is the same plan with the EXT prime dropped). `mxbl=8` is the lowest
qnum we have host-verified; we observed that `mxbl=6` (qnum 27) breaks the
ReSBM plan (see "Why lowering qnum is not a workaround" below), so qnum 30 is
the practical floor for the device path.

### Problem

ResNet20 (CIFAR-10, single image, ReSBM with ~16 bootstraps) uses `mxbl=8`
(host-verified correct, pred = 6, under FLEXIBLEAUTOEXT; FIXEDMANUAL
host-correctness was not separately run -- see Background), which yields
CRT-qnum = 30 under FIXEDMANUAL (31 under FLEXIBLEAUTOEXT; FIXEDMANUAL has no EXT
tower, so qnum = multiplicative_depth + 1). qnum = 30 fits the device CRAM
ceiling (`dsa_bconv_table[PEG_NUM=8][16384]`; cram_total ~12.3k-12.9k < 16384 at
qnum=30, varying with dnum). But the device then cannot run the keyswitch (rotate/relin)
or even complete the bootstrap setup, depending on the HYBRID digit count
`dnum = SetNumLargeDigits`:

| SetNumLargeDigits (dnum) | beta = ceil(qnum/dnum) = ceil(30/dnum) | result |
|---|---|---|
| 4 | 8 | keyswitch VRF allocation fails at the first rotate |
| 6 | 5 | keyswitch VRF allocation fails at the first rotate |
| 8 | 4 | device HBM `Out of device memory` during bootstrap setup |
| 10 | 3 | device HBM `Out of device memory` during bootstrap setup |

`vrf_bank_cap` (`num_polys_per_bank`) at 256x256 peg=8 is 9.

### Why it is a conflict (no `dnum` works at qnum=30 full packing)

- VRF: the rotate/relin keyswitch lowers a digit slice of `beta` polys plus the
  simultaneously-live ciphertext. At qnum=30 (a 30-tower ciphertext) the live set
  does not fit the VRF for `beta = 8` or `5`. The MLIR failure is:
  `VRF allocation failed: cannot place buffer of size 8 poly(s) in any VRF bank
  or memory (insufficient capacity for the set of simultaneously-live buffers)`
  (located at the keyswitch, `kernels/ckks.py` around the `extract_slice` /
  rotate path). Smaller `beta` (higher `dnum`) relieves the VRF.
- HBM: the eval-key material (HYBRID switching keys, one per digit) scales with
  `dnum`. At `dnum = 8` or `10` the bootstrap setup
  (`EvalBootstrapKeyGen` / `SetBootstrapPrecomTo` for 32768 slots) exceeds the
  device HBM and aborts with `MemoryError: Out of device memory`. Lower `dnum`
  (fewer digits) relieves the HBM.
- So VRF wants high `dnum` (small `beta`); HBM wants low `dnum`. At qnum=30 full
  packing the two ranges do not overlap. `peg` is fixed at 8 by the cmodel
  (`dsa_bconv_table[PEG_NUM=8]`), so we cannot raise VRF/HBM that way. The cipher
  size is set by the ring (65536) and the tower count (qnum), not by the packed
  message count, so reducing the slot count is not a lever for the cipher VRF
  (it only reduces bootstrap-key HBM, which is not the binding constraint at the
  low `dnum` where VRF fails).

### Why lowering qnum is not a workaround for resnet20

`mxbl` (compiler `max_bts_lvl`, the ReSBM per-segment consumable budget) is a
compile-time lever. We host-verified `mxbl=8` correct (pred = 6 under
FLEXIBLEAUTOEXT; FIXEDMANUAL not separately run -- see Background). Lowering it
shrinks the chain (qnum drops); we ran `mxbl=6` (qnum=27, which fits the device)
on the resnet20 device graph and the ReSBM plan placed a bootstrap on a
ciphertext already higher than its own landing target, so the bootstrap was a
no-op (observed trace):

```
fhert/openfhe/_ckks_context.py:3489  assert ct.GetQnum() > init_qnum
AssertionError: Bootstrap is a no-op: output qnum=10 <= input qnum=17.
  ... bootstrap cannot refresh towers it didn't have headroom to gain.
```

(On host, OpenFHE silently returns the input in this case, so the prediction
would be wrong.) This no-op is a scheme-plan matter on our side, not a fhert
bug; the fhert assert is correct and useful. We mention it to explain why we
cannot simply lower qnum to fit the device. We did not exhaustively map every
mxbl < 8, but the observation shows the plan breaks below the qnum=30 floor, so
qnum=30 is the practical minimum for the device path -- and qnum=30 full packing
exceeds the 256x256 device VRF/HBM.

### Reproduction (fhert-native, no fhe-cmplr needed)

The VRF failure does not require the full resnet20 graph; it reproduces with one
keyswitch rotate on a qnum=30, full-packing, FIXEDMANUAL context, sweeping
`dnum`. Adapt the import / API names to your current build; the structure
mirrors `examples/profile_bootstrap.py`:

> **NOTE:** the snippet below is INCOMPLETE and does
> NOT reproduce the device failure as written -- it omits two steps required
> to reach the device keyswitch kernel, so `cc.EvalRotate` falls through to
> the HOST rotate and silently prints "rotate OK" for every `dnum`. The two
> required additions are (1) `cc.SetEvalKeysTo(keys.secretKey.GetKeyTag(),
> rotation_indices=[1])` after keygen (uploads the mult+rotate eval keys to
> device HBM and populates the device dispatch's `_rot_key_slots` map) and
> (2) `ct_dev = ct.to(dev)` so `EvalRotate` routes through the
> `@device_patch` device path. The verified, corrected single-config minimal
> reproducer is `doc/fhert-device-fix/repro_vrf_alloc.py` (same sweep logic
> + both fixes, fhert-native, runs quickly). Run:
> `FHERT_ROOT=/workspace/fhert XDG_CACHE_HOME=/tmp/fhert_cache_dev
> /opt/ace-env/bin/python3 -u doc/fhert-device-fix/repro_vrf_alloc.py 65536 28 4`
> -> qnum=29/dnum=4 raises "cannot place buffer of size 8" (minimal failing
> qnum at dnum=4; qnum=28/dnum=4 with alpha=7 PASSES -- the VRF is qnum-driven,
> failure buffer size = `alpha = ceil(qnum/dnum)`).

```python
# VRF probe: build a FIXEDMANUAL qnum=30 device context and do ONE EvalRotate.
from fhert.openfhe import (CCParamsCKKSRNS, GenCryptoContext, FIXEDMANUAL,
    KeySwitchTechnique, PKE, KEYSWITCH, LEVELEDSHE)
from fhert.backends import CModelDevice, MPUH1Config

RING = 65536
MUL_DEPTH = 29          # FIXEDMANUAL: CRT-qnum = mul_depth + 1 = 30
FIRST = 60; SF = 59
dev = CModelDevice(MPUH1Config(256, 256, 1, 8))

for dnum in (4, 6, 8, 10):
    p = CCParamsCKKSRNS()
    p.SetScalingTechnique(FIXEDMANUAL)
    p.SetKeySwitchTechnique(KeySwitchTechnique.HYBRID)
    p.SetRingDim(RING)
    p.SetMultiplicativeDepth(MUL_DEPTH)
    p.SetFirstModSize(FIRST)
    p.SetScalingModSize(SF)
    p.SetNumLargeDigits(dnum)           # beta = ceil(30 / dnum)
    p.SetBatchSize(RING // 2)           # full packing, 32768 slots
    cc = GenCryptoContext(p)
    for f in (PKE, KEYSWITCH, LEVELEDSHE):
        cc.Enable(f)
    cc.SetParamsTo(dev)
    keys = cc.KeyGen()
    cc.EvalMultKeyGen(keys.secretKey)
    cc.EvalRotateKeyGen(keys.secretKey, [1])
    pt = cc.MakeCKKSPackedPlaintext([1.0] * (RING // 2))
    ct = cc.Encrypt(keys.publicKey, pt)
    beta = (30 + dnum - 1) // dnum
    try:
        cc.EvalRotate(ct, 1)            # keyswitch; this is where VRF allocates
        print(f"dnum={dnum} beta={beta}: rotate OK")
    except Exception as e:
        print(f"dnum={dnum} beta={beta}: {type(e).__name__}: {str(e)[:160]}")
```

Expected: `dnum=4`/`6` raise `MLIRError` (`VRF allocation failed: cannot place
buffer of size N poly(s) ...`). `dnum=8`/`10` may pass the rotate but the
bootstrap setup (below) runs out of device memory at this qnum; run the HBM
probe to see it:

```python
# HBM probe: bootstrap setup at qnum=30 (EvalBootstrapSetup + keygen +
# SetBootstrapPrecomTo). Reuse make_bootstrap_context-style params but force
# qnum=30 (mul_depth chosen so depth+bts gives CRT-qnum 30) and sweep dnum.
# Expected at dnum=8/10: MemoryError: Out of device memory during
# EvalBootstrapKeyGen / SetBootstrapPrecomTo (32768 slots).
```

(Use your `examples/profile_bootstrap.py` `make_bootstrap_context` as the base,
set `SetScalingTechnique(FIXEDMANUAL)`, FLEXIBLEAUTOEXT-free mul_depth that
yields CRT-qnum 30, `SetNumLargeDigits(dnum)`, ring 65536, full packing 32768,
and call `EvalBootstrapSetup` + `EvalBootstrapKeyGen` + `SetBootstrapPrecomTo`.)

### Reproduction (end-to-end, via fhe-cmplr; verified)

This is the exact path that produced the table above. It needs the fhe-cmplr
build (`fhe_cmplr`) and the resnet20 onnx. Compile with
`mxbl=8` (so the emitted chain is qnum=30) and df on, then run under `rt_fhert`
device, sweeping `FHE_DEV_NUM_LARGE_DIGITS`:

```bash
mkdir -p /tmp/r20 && cd /tmp/r20
fhe_cmplr \
  /workspace/fhe-cmplr/onnx-models/ace/resnet20_cifar10_pre.onnx \
  -CKKS:hw=192:q0=60:sf=56:sbm:mxbl=8 \
  -SIHE:relu_vr_def=3:relu_vr=/relu/Relu=4;/layer1/layer1.0/relu_1/Relu=4;...;/layer3/layer3.2/relu_1/Relu=20 \
  -CKKS2PY:rt=rt_fhert -CKKS2PY:df=weights.dat -o out.py
for D in 4 6 8 10; do
  FHE_DEV_SCALING=fixed FHE_DEV_DEBUG=1 FHE_DEV_NUM_LARGE_DIGITS=$D \
    FHE_BTS_SF=59 FHE_BTS_CORRECTION=13 \
    FHE_DEVICE=1 FHE_DEVICE_RING=65536 FHE_DEVICE_HW=256,256,1,8 \
    FHERT_ROOT=/workspace/fhert XDG_CACHE_HOME=/tmp/fhert_cache_dev \
    /opt/ace-env/bin/python3 -c "
import sys; sys.path.insert(0,'/workspace/fhe-cmplr/test')
sys.path.insert(0,'/workspace/fhe-cmplr/test/rtlib_py/py_verify')
import run_py_accuracy as rpa; from rtlib_py.common.driver import _seed_inputs
ns,prov,sch=rpa._setup('out.py','rt_fhert',strict=True)
prov._INPUTS={};prov._OUTPUTS={}
data,_=rpa.cifar_load('/workspace/datasets/cifar-10-batches-bin/data_batch_1.bin',0,10)
_seed_inputs(prov,{'inputs':{'input':data}},sch)
ns['Main_graph']()
" 2>&1 | grep -E 'FHE_DEV_DEBUG|MLIRError|cannot place|Out of device memory|qnum='
done
```

`FHE_DEV_DEBUG` prints, for each dnum:
`ring=65536 hw=(256,256,1,8) qnum=30 pnum=.. dnum=D beta=.. vrf_bank_cap=9
cram_total=<12.3k-12.9k>` (cram_total varies with dnum, all < 16384; e.g. 12734
at dnum=4). `dnum=4`/`6` raise `MLIRError: VRF allocation failed: cannot place
buffer of size N`; `dnum=8`/`10` raise `MemoryError: Out of device memory`
during setup. (`FHE_DEV_SCALING=fixed` selects FIXEDMANUAL; with FLEX, qnum is
31 and the same conflict holds.)

### Refinement: the fhe-cmplr-side qnum levers bottom out at 30

After adding FLEXIBLEAUTO (no-EXT) support and sweeping the per-relu depth and
the ReSBM post-bootstrap budget (`mxbl`) on the host path, all fhe-cmplr-side
qnum levers for resnet20 bottom out at CRT-qnum ~30 (no-EXT) / ~31 (EXT), i.e.
the VRF/HBM ceiling above is NOT cleared from our side:

- no-EXT drops exactly 1 tower (qnum 31 -> 30 at `mxbl=8`); fhert validates
  no-EXT on the device bootstrap path (`tests/ckks/test_bootstrap.py #99`), and
  resnet20 host no-EXT at `mxbl=8` is host-correct (pred=6, qualitatively close to
  onnxruntime; `RESULTS.md` sec.9). So no-EXT is sound but saves only 1 tower.
- `relu_depth` (`-SIHE:relu_depth`, global, registered values {9,11,13}) CANNOT
  be lowered for resnet20: only `depth=11` Chebyshev (default) is numerically
  valid; `depth=9` POWER -> rt_ref all-NaN (the low-degree composite sign
  diverges on resnet20's wide value-range relus, e.g. `.../Relu=20`);
  `depth=9` Chebyshev -> compile crash (`include/fhe/util/app_composite_poly.h:179`
  `AIR_ASSERT iter != _app_sign_comp_poly.end()`, the {CHEB,9} combo is not
  registered); `depth=13` Chebyshev -> NaN. So the architecture team's "relu 7"
  is not reachable in fhe-cmplr (the minimum registered, depth-9 POWER,
  diverges), and lowering the per-relu depth is NOT a usable lever.
- `mxbl` (post-bootstrap budget) floor on host no-EXT is 8: `mxbl=8` is
  host-correct; `mxbl=7` and `mxbl=6` both raise `[hard] rescale_ciph: level
  underflow: 0-1 < 0` (the `ir_capacity` chain is too shallow; a rescale runs
  below level 0). So the post-bootstrap budget cannot go below 8 with
  `relu_depth=11`.

Net: with no-EXT + `mxbl=8` + `relu_depth=11`, resnet20's qnum = 8 + bts_depth +
R + 1 ~= 30 (bts_depth ~19, R=2). The single remaining qnum lever is the
fhert-side bootstrap depth itself (bts_depth ~20, OpenFHE-native). The
architecture team's sparse bootstrap consumes 17. Reducing the fhert device
bootstrap depth 20 -> 17, combined with no-EXT + `mxbl=8`, gives qnum ~= 8 + 17
+ 2 + 1 = 28. Whether qnum=28 clears the device VRF/HBM empty-`dnum` window
above is uncertain (the per-keyswitch VRF footprint `beta` may be `dnum`-driven
and qnum-invariant; the HBM and CRAM gains are definite) and must be re-measured
by re-running the Issue-1 `dnum` sweep at qnum=28.

### fhe-cmplr team's recommended direction: fhert fixes its device VRF allocation to handle large qnum (preferred over per-model qnum reduction)

The refinement above (lowering the fhert bootstrap depth 20 -> 17 to reach
qnum ~28 for resnet20) is a *model-specific* duck-under: it shrinks one
model's qnum to fit the device's *current* VRF/HBM ceiling. It does not raise
the ceiling, so fhert stays limited to small-qnum plans and cannot integrate
the broader class of upper-layer models -- any deep ReSBM plan, resnet110 and
larger CNNs, or any model whose host-correct plan needs qnum >= ~30 at full
packing would still hit the same wall.

The fhe-cmplr team's *preferred* fix is therefore the device-side one: make
the CModelDevice run at large qnum, so fhert is a general backend rather than a
small-qnum-only one. This is the VRF/HBM allocation problem at the heart of
Issue 1: at large qnum full packing, no `dnum` works (low `dnum` -> the
keyswitch digit slice `beta` plus the simultaneously-live ciphertext exceeds
the per-bank VRF; high `dnum` -> the HYBRID eval-key material exceeds HBM),
i.e. the VRF-relief and HBM-relief `dnum` ranges have an empty overlap at large
qnum. The fix is fhert's own device keyswitch **VRF allocation** -- how it *uses* the
hardware VRF -- so a working `dnum` always exists at large qnum. This is NOT a
VRF-size change: the CModelDevice is a faithful simulator of the hardware VRF
(per-bank capacity, bank count, and peg are hardware-decided and merely mirrored
by the cmodel), so the VRF cannot be enlarged on the cmodel/fhert side. fhert's
lever is its keyswitch allocation logic within that fixed VRF: let a digit slice
be placed *across* banks rather than within one, and/or a HYBRID decomposition
that keeps the per-keyswitch footprint `beta` small without multiplying the
eval-key HBM footprint -- so the VRF-relief and HBM-relief `dnum` ranges overlap
at large qnum. These directions are listed in "What we would like the fhert team
to look at" below.

This generalizes: every deep/large model that cannot fit today (resnet20 at
qnum 30 now; resnet110 / larger plans next) becomes runnable, and the
bootstrap-depth reduction becomes an optimization rather than a prerequisite.
We recommend the fhert team prioritize this device-capacity fix for broad
upper-layer integration, and treat the bootstrap-depth 20 -> 17 ask as a
parallel near-term complement that gets resnet20 specifically under the
current ceiling.

Diagnostic caveat: our source audit could not fully confirm whether the
binding per-keyswitch VRF footprint `beta` is qnum-driven (`ceil(qnum/dnum)`,
as the Issue-1 table frames it) or `dnum`-driven (~`dnum`, qnum-invariant).
This affects *which* device knob to turn, not *the* fix direction. The exact
qnum at which the `dnum` window opens/closes should be pinned by re-running the
Issue-1 `dnum` sweep at qnum 28 (once a 17-deep bootstrap is available) and at
a few larger qnum. Either way, the device VRF/HBM must accommodate large qnum.

### What we would like the fhert team to look at

The device keyswitch (rotate/relin) VRF allocation cannot place a `beta`-poly
digit slice together with a qnum=30 ciphertext at 256x256 / peg=8, and the
bootstrap setup HBM cannot hold the eval-key material for high `dnum` at 32768
slots. This is fhert's own device keyswitch VRF *allocation* to fix -- the VRF
size, bank count, and peg are hardware-decided and faithfully mirrored by the
cmodel (a simulator), so they are not to be enlarged here; the directions are to
let the keyswitch place the digit slice across banks rather than within one,
and/or a HYBRID decomposition that keeps `beta` small without multiplying the
eval-key HBM footprint (so the VRF-relief and HBM-relief `dnum` ranges overlap
at qnum=30). ResNet20 at qnum=30 full
packing is the target; the qnum=27 reference in `examples/profile_bootstrap.py`
(half packing, single bootstrap) fits, but resnet20 needs qnum=30 and full
packing for a host-correct ReSBM plan.

Additionally: reduce the fhert device bootstrap
multiplicative depth from the OpenFHE-native ~20 toward the architecture team's
sparse-bootstrap 17 (resnet20 hand-written sparse bootstrap consumes 17 levels;
"bootstrap uses openfhe's own, no modification needed" -- likely a
`level_budget` / sparse-secret configuration rather than a kernel rewrite). This
is the single remaining fhe-cmplr-visible lever to lower resnet20's qnum below
30: combined with FLEXIBLEAUTO no-EXT + `mxbl=8` it reaches qnum ~28, the range
that might clear the 256x256 VRF/HBM window. We will re-run the `dnum` sweep
above at qnum=28 once a <=17-deep bootstrap is available.

## Issue 2: sharded / sub-scaled-factor vectorization explodes the distinct device-kernel count; the cmodel cold-JIT cost is then prohibitive

### Problem

Three regress58 models time out in the device sweep (per-model run) not
because of a correctness error but because the device kernel cold-JIT cost,
multiplied by the number of distinct kernels, is hours:

| model | vectorization flag | emitted N | observed |
|---|---|---|---|
| conv/resnet18-conv2d_1x16x28x28_28_14_7 | `-VEC:ms=4096:sharding` | 8192 | ~1000+ distinct kernels; per-kernel cold-JIT; exceeds the sweep timeout |
| op_block/resnet_block2_hw16 | `-VEC:ms=32768:ssf` | 65536 | same kernel-count/JIT limit at the largest grid |
| op_block/resnet_block2_hw32 | `-VEC:ms=32768:ssf` | 65536 | same |

The sharded / sub-scaled-factor (`ssf`) vectorization emits a large number of
distinct rotation indices (grid-aligned rotations such as
`rotate_ciph(result, num_grid * 256)`), and each distinct rotation/keyswitch
identity is a separate cmodel kernel that is cold-JIT-compiled once into a
`kernel.bin` subprocess. The kernels themselves compile and run correctly (the
process is in R state, advancing through the graph), and the same graph on host
finishes quickly (no device JIT). So this is a device-kernel-JIT performance
ceiling, not a correctness bug. The other 55 regress58 models (non-sharded, or
sharded with a small kernel count) run fine on device under FIXEDMANUAL.

### Reproduction (end-to-end, via fhe-cmplr; verified)

```bash
mkdir -p /tmp/scan && cd /tmp/scan
fhe_cmplr \
  /workspace/fhe-cmplr/onnx-models/conv/resnet18-conv2d_1x16x28x28_28_14_7.onnx \
  -VEC:ms=4096:sharding -CKKS:q0=60:sf=59:N=8192 \
  -CKKS2PY:rt=rt_fhert -CKKS2PY:df=weights.dat -o out.py
FHE_DEV_SCALING=fixed FHE_DEV_DEBUG=1 \
  FHE_DEVICE=1 FHERT_ROOT=/workspace/fhert \
  XDG_CACHE_HOME=/tmp/fhert_cache_dev PYTHONWARNINGS=ignore \
  /opt/ace-env/bin/python3 -c "
import sys; sys.path.insert(0,'/workspace/fhe-cmplr/test')
sys.path.insert(0,'/workspace/fhe-cmplr/test/rtlib_py/py_verify')
import run_py_accuracy as rpa; from rtlib_py.common.driver import _seed_inputs
ns,prov,sch=rpa._setup('out.py','rt_fhert',strict=True)
prov._INPUTS={};prov._OUTPUTS={}
import onnxruntime as ort, numpy as np
x=np.random.RandomState(0).randn(1,1,28,28).astype('float32')
exp=ort.InferenceSession('/workspace/fhe-cmplr/onnx-models/conv/resnet18-conv2d_1x16x28x28_28_14_7.onnx').run(None,{'input.1':x})
_seed_inputs(prov,{'inputs':{'input.1':x.reshape(-1).tolist()}},sch)
ns['Main_graph']()
"
# meanwhile, in another shell, watch the kernel count grow (many new kernel.bin
# per minute; > 1000 distinct kernels total; per-kernel cold-JIT):
find /tmp/fhert_cache_dev -name kernel.bin | wc -l
```

(The exact input tensor name / shape depends on the model; the point is to
drive `Main_graph` on device and watch the distinct-kernel count and the
per-kernel cold-JIT time. The run does not error; it just does not finish in a
practical time.)

### What we would like the fhert team to look at

The per-kernel cold-JIT cost times the distinct-kernel count (1000+ for
sharded convs) makes the device path impractical for sharded / `ssf` workloads.
Possible directions: a persistent, cross-run device-kernel JIT cache (so a
second run reuses compiled `kernel.bin`s instead of re-JIT-ing); a faster
cold-JIT path; or reducing the number of distinct kernels a sharded graph emits
(kernel identity coalescing for rotation indices that produce the same MLIR).

## What is NOT a fhert issue (for completeness)

- The `mxbl=6` bootstrap no-op in Issue 1's "why lowering qnum is not a
  workaround" is a compiler ReSBM-plan matter on our side (the plan places a
  bootstrap where the ciphertext is already higher than the target), not a fhert
  bug; the fhert assert at `_ckks_context.py:3489` is correct and useful.
- The `EvalAdd` "1-component" blocker that previously stopped resnet20 at the
  first `add_ciph` is already fixed on our side (`rt_fhert` no longer forces
  FLEXIBLEAUTOEXT-style nsd tracking under FIXEDMANUAL); see `RESULTS.md`. The
  resnet20 failures that remain are the device VRF/HBM in Issue 1.
- The CRAM `dsa_bconv_table[16384]` ceiling (qnum >= 33 overflows the bconv
  table; the current cmodel raises at `upload_bconv_table`, the retired former
  `set_params` core-dumped -- see the Issue 1 background) is not the current
  resnet20 blocker: with `mxbl=8` resnet20 is CRT-qnum 30, below the ceiling.
  The current blocker is the VRF/HBM conflict in Issue 1.

## Issue 3: FLEXIBLEAUTO (no-EXT) diverges from FLEXIBLEAUTOEXT at level-0 add_plain (toy models only; not fhe-cmplr-fixable)

### Problem

Under `ScalingTechnique=FLEXIBLEAUTO` (no EXT) on the device path, two toy
regress58 models raise an exception during execution: `op/add_const.onnx` and
`op/add_const_1_dimension.onnx` (a plain add of a constant to a depth-0
ciphertext at level 0). Every other regress58 model (49 PASS + 4 NOISY,
including resnet20 host no-EXT) runs correctly under no-EXT. The failure is
no-EXT-specific (FLEXIBLEAUTOEXT and FIXEDMANUAL pass these two models),
df-independent, and qnum-independent (it also fails at qnum=2; `op/add` passes
at qnum=1).

### Root cause (fhert binding, read-only for us)

`fhert/openfhe/_ckks_context.py:1208-1231` (the plain/scalar encoding path) has
FLEXIBLEAUTOEXT-only branches at level 0:

- at level 0, FLEXIBLEAUTOEXT uses `GetScalingFactorRealBig(level)` and returns
  early (no depth multiplication); FLEXIBLEAUTO (no-EXT) takes the else branch,
  `GetScalingFactorReal(level)`, and proceeds to the depth-multiplication path.

`add_const` does `add_plain` on a fresh (level-0) ciphertext, so under no-EXT the
plaintext is encoded at a divergent scale and the resulting ciphertext is
malformed; the cmodel then aborts (the `CKKSContext.__del__` `mem_free` "double
free" is teardown, not the root cause). Models whose `add_plain` runs after
rescales (level < 0), and cipher+cipher adds (`op/add`), do not trigger it --
confirmed by resnet20 host no-EXT being correct (pred=6).

### Why it is not a fhe-cmplr bug and not blocking

- It is in the read-only fhert binding, not in `rt_fhert`. `rt_fhert` correctly
  selects `FLEXIBLEAUTO`; the divergence is the binding's level-0 handling.
- It affects only toy add-constant-at-level-0 models. No real model (conv / relu
  / resnet) does `add_plain` at level 0 (its `add_plain` follows rescales), so
  resnet20 device/host no-EXT is unaffected.
- The default device scaling stays FLEXIBLEAUTOEXT (byte-identical baseline),
  under which these two models pass; no-EXT is an opt-in mode.

### What we would like the fhert team to look at

Make the level-0 plain-encoding path (`_ckks_context.py:1208-1231`) consistent
between FLEXIBLEAUTO and FLEXIBLEAUTOEXT (let no-EXT also use the big scaling
factor / no-depth-mult path at level 0, or confirm the no-EXT else branch is
intended and document the level-0 add_plain caveat). Low priority: toy models
only; resnet20 and the rest of regress58 are correct under no-EXT.
