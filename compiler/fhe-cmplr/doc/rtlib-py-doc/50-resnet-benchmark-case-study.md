# resnet20 on real CKKS: root causes and env-gated fixes

resnet20_cifar10 inference on real CKKS (`rt_openfhe` / `rt_fhert` host + FIXEDMANUAL + ReSBM bootstrap) is verified correct after three env-gated fixes concentrated in `rtlib-py/rtlib_py/rt_openfhe/__init__.py`: `pred=3=label=3`, logits matching the `rt_ref` plaintext baseline (same argmax). The raw resnet-batch tallies (the 18-cell grid) are in [30-test-status.md](30-test-status.md#resnet-batch-detail); this document is the root-cause + fix + rerun reference, not a per-run snapshot. The generalized limitation is in [40-open-issues.md](40-open-issues.md#known-compiler-defects).

## Env-gated fixes

All three are env-gated and default to green-gate parity (the regress corpus inserts no ReSBM bootstrap, so it is unaffected).

| Fix | Treats | How to enable | Mechanism |
|---|---|---|---|
| **nsd-reset** | nsd blindness (crash) | in-code (`bootstrap_ciph` / `raise_mod`), automatic | after `EvalBootstrap`, rescale until `nsd=1` and track the real `NoiseScaleDeg` (mirrors ant `bootstrap.c`) |
| **`FHE_BTS_RESERVE=8`** | bottom-bootstrap has no headroom (crash) | env, default `0` | add an R=8 tower below IR 0, decoupled from the IR mapping, giving `EvalBootstrap` downward headroom |
| **`FHE_BTS_SF=59`** | FIXEDMANUAL scaling-drift (garbage) | env, default the emitted `56` | use the openfhe 64-bit FIXEDMANUAL baseline (59/60), enlarging primes and reducing per-rescale drift |

Concepts: `nsd` = `NoiseScaleDeg` (pending scale depth; bootstrap requires 1); R-tower = an extra prime added below IR 0 purely as bootstrap headroom, not part of the IR mapping; `SF` = scaling-factor bit-size -- under FIXEDMANUAL, larger SF means `q_i` is closer to `delta = 2^sf`, reducing drift.

## Findings and analysis

Under the default config, resnet20 on `rt_openfhe` either crashed or produced garbage (wrong prediction). Three root causes, all from how `rt_openfhe` bootstraps relative to the ant/openfhe design:

**A. nsd blindness -> crash.** openfhe `EvalBootstrap` leaves a real `NoiseScaleDeg=2`; the old `rt_openfhe` code discarded it and hardcoded the tracked nsd to 1. Downstream `mul` / `rescale` guards went blind, the real nsd doubled each round past the RNS tower, and `SetLevel` raised. **Fix**: nsd-reset (auto) -- rescale until `nsd=1` and track the real nsd.

**B. bottom-bootstrap has no headroom -> crash.** The `.py`'s ReSBM scheduling drives the cipher to the chain bottom (IR 1, one tower) then bootstraps. ant re-mints the tower before `Bootstrap`; openfhe `EvalBootstrap` instead consumes level downward at the input level, so a one-tower cipher has no down-headroom and `DropLastElement` crashes. Deepening the chain does not help: IR 1 = `target_IR - critical_path`, independent of the prime count. **Fix**: `FHE_BTS_RESERVE=8` -- add an R=8 tower below IR 0 as real downward headroom.

**C. FIXEDMANUAL scaling-drift -> garbage (the correctness cause).** openfhe `AdjustCiphertext`'s FIXEDMANUAL branch does not read the cipher's actual scale (FLEXIBLEAUTO reads `sourceSF` / `targetSF` and actively compensates); it blindly applies the correction assuming the canonical scale `delta`. Since `q_i != delta`, each rescale accumulates drift `prod(delta/q_i)` that FIXEDMANUAL never compensates. resnet20's deep chain (20 bootstraps at deep levels) accumulates large drift, so `EvalMod` cannot cancel `qI`, the bootstrap refreshes only a few bits, and the degree-20 Chebyshev relu amplifies inputs that drifted out of the normalized interval `[-1,1]` to garbage. **Fix**: `FHE_BTS_SF=59` -- use the openfhe 64-bit FIXEDMANUAL baseline (59/60), enlarging primes, reducing per-rescale drift, and improving the refresh enough to cross the degree-20 relu explosion threshold.

The three fixes are all in `rtlib-py/rtlib_py/rt_openfhe/__init__.py`, env-gated, and default to green-gate parity. Why it matters: without nsd tracking, bottom headroom, and drift compensation, `rt_openfhe` cannot run deep ReSBM nets; with them, resnet20 reaches the plaintext-baseline result.

## Results

With `FHE_BTS_SF=59 FHE_BTS_RESERVE=8`, resnet20 image 0 on `rt_openfhe` predicts `pred=3=label=3` and the logits match the `rt_ref` plaintext baseline (same argmax). After the three fixes, resnet20 on `rt_openfhe` went from "crash / garbage output" to "correct output matching the plaintext baseline". Per-run `max_abs` and timing are run snapshots and are not recorded here.

## Reproduction

Set the env vars and run `test/ace_test.py` against a built `fhe_cmplr` with the Python path selected:

```bash
# rt_openfhe (primary real-CKKS path)
FHE_BTS_SF=59 FHE_BTS_RESERVE=8 OPENFHE_PY_BUILD_DIR=<openfhe-python/build> \
  python3 test/ace_test.py <model>.onnx --backend rt_openfhe --df on -o -n 1 -i 0

# rt_fhert host mode (same fixes; FHE_BTS_SF=59)
FHERT_ROOT=<fhert> FHE_BTS_SF=59 \
  python3 test/ace_test.py <model>.onnx --backend rt_fhert --df on -o -n 1 -i 0

# rt_ref plaintext baseline (no bootstrap, no env vars)
python3 test/ace_test.py <model>.onnx --backend rt_ref --df on -n 1 -i 0
```

`--backend rt_openfhe --df on` selects the Python path; without `--backend` it is the original C path. The compile/run workflow and the `df` argument are in [10-new-backend-integration.md](10-new-backend-integration.md). The resnet-batch accuracy harness produces the raw tallies recorded in [30-test-status.md](30-test-status.md#resnet-batch-detail). The generalized limitation -- `rt_openfhe` bootstrap needs FIXEDMANUAL scaling-drift compensation, nsd tracking, and bottom-bootstrap headroom for deep nets, with these env-gated fixes as the current workaround -- is in [40-open-issues.md](40-open-issues.md#known-compiler-defects).

## Evaluation scope

Coverage limits of this evaluation (not generalized compiler limits):

- Only **resnet20 image 0** is verified correct. No full-dataset accuracy is claimed, and this does not extrapolate to other resnet20 images or to resnet32 / resnet44 / resnet56 / resnet110.
- **resnet110** has a separate `rt_ref` float64-overflow issue (degree-20 relu intermediate exceeds float64; `rt_ref` is not changed), outside this fix's scope -- see [40-open-issues.md](40-open-issues.md#unsupported-configuration-upstream-read-only).
- The **18-cell resnet batch was not rerun** after the fix, so 15 batch cells remain untested -- see [30-test-status.md](30-test-status.md#resnet-batch-detail).
- **Single-image performance** is not usable throughput (R=8 chain plus ~20 bootstraps) -- see [40-open-issues.md](40-open-issues.md#performance-and-scalability).
