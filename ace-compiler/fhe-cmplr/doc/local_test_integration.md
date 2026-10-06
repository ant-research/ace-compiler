# local_test integration: running the cti regression harness against a built fhe_cmplr

This document explains how to bind the `cti` repository's `local_test` framework to a built
`fhe_cmplr` in **this** checkout, and run operator/model regression tests against it. It also
records the hook-vs-skill decision for the Claude integration that ships with this repo.

`local_test` lives in a **separate** repository (`air-infra/cti`). It is not part of this
compiler tree; this document only describes how to point it at this compiler's build
artifacts.

Path conventions: `${FHEROOT}` denotes the **workarea root** -- the directory that holds the
three source repos (`air-infra`, `nn-addon`, `fhe-cmplr`) and the build dir as siblings (the
"clone these repos to the same directory" layout from `BUILD.md`), and is the value passed to
`detect_env.sh --root`. `${FHE_BUILD_DIR}` is the build dir (`${FHEROOT}/build`); `${FHE_CMPLR}`
is the compiler binary (`${FHE_BUILD_DIR}/driver/fhe_cmplr`). The dev-container `/app` root is
the same shape, just under one root.

---

## 1. What local_test is, and what it adds over this repo's own `test/`

`local_test` is the operator- and model-level FHE-compiler regression harness. Each case is a
single ONNX operator (Conv, Pool, GAP, Gemm) extracted from LeNet / ResNet-20, or a whole
model (LeNet, ResNet-20). Every case runs across a matrix of named compiler option sets
(optsets), and each (case x optset) is classified `PASS` or `FAIL` with the reason recorded.

This repo's own `test/test_fhe_cmplr.sh` is **compile-only**: it loops `fhe_cmplr *.onnx -o *.c
-P2C:lib=ant` and stops there (no link, no run, no expected-output check). `local_test` is a
strict superset of that one step and adds the rest of a real regression:

```
*.py --(gen_onnx_and_main.sh)--> .onnx + main.c   (backend-agnostic; torch+onnx+onnxruntime)
fhe_cmplr onnx -o graph.c <opts>                   (compile; fhe_build.sh)
cc main.c graph.c <rtlib includes/libs> -> exe      (link)
exe -> SUCCESS! / FAILED!                          (run + accuracy verify; fhe_run.sh)
results.tsv + summary                              (run_test.sh orchestrator)
```

So `local_test` complements `test/test_fhe_cmplr.sh` rather than duplicating it: same compile
step, plus link + run + onnxruntime golden + per-element error check + matrix + reporting.

---

## 2. Layouts

`local_test`'s `detect_env.sh` finds the compiler by probing a single **root** and expects, under
that root, the binary, the rtlib archives, and the rtlib headers (by sentinel file names). Two
layouts satisfy this:

### 2.1 Dev container `/app` layout (canonical; matches `cti`'s `env.sh.example`)

Everything under one root `/app`: binary at `/app/build/driver/fhe_cmplr`, rtlib archives at
`/app/build/rtlib/build/{ant,common}/libFHErt_*.a`, rtlib/aair/nn headers under
`/app/fhe-cmplr/rtlib/include`, `/app/air-infra/include`, `/app/nn-addon/include`. A single
`detect_env.sh --root /app` binds the whole harness. This is what the `cti` docs assume.

### 2.2 Sibling-layout workarea (the `BUILD.md` layout)

This checkout places the three source repos and the build dir as **siblings** under
`${FHEROOT}`:

```
${FHEROOT}/
  air-infra/        -> ${FHEROOT}/air-infra/include/air/base/rt_timing.h
  nn-addon/         -> ${FHEROOT}/nn-addon/include/nn/core/rt_timing.h
  fhe-cmplr/        -> ${FHEROOT}/fhe-cmplr/rtlib/include/common/rtlib.h
          rtlib/ant/include/ckks/cipher.h
          include/fhe/core/rt_timing.h
  build/            -> ${FHEROOT}/build/driver/fhe_cmplr        (the binary)
          build/rtlib/build/ant/libFHErt_ant.a  (+ common)
          build/rtlib/build/_deps/uthash-src/src/uthash.h      (FetchContent)
```

The binary is in `${FHEROOT}/build` (a sibling of, not a child of, `fhe-cmplr/`), but the
**root for `detect_env` is `${FHEROOT}`**, not `fhe-cmplr`, because both the binary
(`{root}/build/driver/fhe_cmplr`) and the headers (`{root}/fhe-cmplr/rtlib/include`,
`{root}/air-infra/include`, `{root}/nn-addon/include`) resolve relative to `${FHEROOT}`.

ACE/GPU: `FHE_ENABLE_ACE` defaults to `OFF`, so this build has no `libFHErt_ace.a`. The CPU (ant)
flow is fully usable now; the GPU (ACE) flow requires rebuilding with `-DFHE_ENABLE_ACE=ON`
(section 6).

---

## 3. Prerequisites

- A successful build of `fhe_cmplr`. From `BUILD.md`:
  ```
  cmake -S fhe-cmplr -B build -DFHE_WITH_SRC="air-infra;nn-addon" \
        -DCMAKE_BUILD_TYPE=Release -DFHE_CODE_CHECK=OFF -DNN_CODE_CHECK=OFF -DAIR_CODE_CHECK=OFF
  cmake --build build -- -j$(nproc)
  ```
  Here the build directory is `${FHE_BUILD_DIR}`. The CPU (ant) flow needs **only** the build
  (no `cmake --install`): `detect_env.sh` reads headers from the source tree and archives from
  `${FHE_BUILD_DIR}/rtlib/build/{ant,common}/`.

- Python generation deps on the frontend host: `torch`, `onnx`, `onnxruntime` (and `onnxsim`
  for ad-hoc conv generation). **Important**: every generator exports with `dynamo=False`; do
  not remove it. The torch >=2.9 dynamo path emits opset-18 graphs that `fhe_cmplr`
  mis-computes silently (a cti finding; `dynamo=False` avoids it).

- The `cti` repo present.

---

## 4. Binding: detect_env

This is the single "connection" step. Pick the workarea root and probe:

```bash
cd ${FHEROOT}/cti/local_test
./tool/detect_env.sh --root ${FHEROOT} --force     # probes + writes tool/env.sh
./tool/detect_env.sh --check                       # self-test: fhe_cmplr runs, headers compile, libs exist
```

With `--root ${FHEROOT}`, the resolved `env.sh` looks like (paths relative to `${FHEROOT}`):

```
FHE_CMPLR      = ${FHEROOT}/build/driver/fhe_cmplr
RTLIB_INCLUDES = -I${FHEROOT}/fhe-cmplr/rtlib/include -I${FHEROOT}/fhe-cmplr/rtlib/ant/include \
                 -I${FHEROOT}/build/rtlib/build/_deps/uthash-src/src -I${FHEROOT}/air-infra/include \
                 -I${FHEROOT}/fhe-cmplr/include -I${FHEROOT}/nn-addon/include
RTLIB_LIBS     = ${FHEROOT}/build/rtlib/build/ant/libFHErt_ant.a \
                 ${FHEROOT}/build/rtlib/build/common/libFHErt_common.a \
                 /usr/lib/x86_64-linux-gnu/libgmp.so /usr/lib/x86_64-linux-gnu/libm.so
RTLIB_CFLAGS   = -DRTLIB_SUPPORT_LINUX
```

`--check` self-test:
```
ok: fhe_cmplr runs: ${FHEROOT}/build/driver/fhe_cmplr
ok: public headers compile (common/rtlib.h, rt_ant/rt_ant.h)
ok: all libs in RTLIB_LIBS exist
env.sh looks good. Next: ./tool/run_test.sh i64_o10
```

`env.sh` uses `: "${VAR:=default}"`, so an already-set environment variable wins without editing
the file (CI-friendly), e.g. `FHE_CMPLR=/some/other/fhe_cmplr ./tool/run_test.sh i64_o10`. If
auto-detection is incomplete for some other layout, fall back to
`cp tool/env.sh.example tool/env.sh` and edit just the four lines (`FHE_CMPLR`,
`RTLIB_INCLUDES`, `RTLIB_LIBS`, `RTLIB_CFLAGS`).

---

## 5. Running tests

The orchestrator is `run_test.sh`. Selection, optset precedence, and exit codes are documented
in the `cti/local_test/README.md` and `doc/usage_guide.md`; the essentials:

```bash
./tool/run_test.sh --list                 # cases, groups, optsets, defaults
./tool/run_test.sh i64_o10 -o native      # smallest smoke (default case_opts is resnet_n64k, so pass -o native for a fast run)
./tool/run_test.sh lenet -o native,lenet_n32k   # group x two optsets
./tool/run_test.sh gemm                   #/fast group
./tool/run_test.sh m_lenet                # whole model (minutes)
./tool/run_test.sh 'i16x32x32_*' --dry-run      # preview commands without running
./tool/run_test.sh --regen i64_o10        # force regenerate .onnx+main.c
```

Optset precedence: `-o` (CLI) wins, else the per-case `case_opts` override, else
`default_optsets` (`native`). Model cases (`m_<name>`) and large-N operator defaults need the
model-derived optsets (`lenet_n32k`, `resnet_n64k`, `lenet_base`, `resnet_base`), because
`native` defaults to a tiny ring that only works for the smallest cases.

`run_test.sh` writes a per-(case x optset) row to `run/results.tsv` and exits 0 iff every
selected (case x optset) PASSes. Per-(case x optset) artifacts land under
`run/<case>.<optset>.{onnx.c,out,build.log,run.log}` (gitignored).

Result enum: `PASS | CMPLR_FAIL | LINK_FAIL | BUILD_PREREQ | RUN_FAIL | CRASH | RUN_PREREQ |
GEN_FAIL`. Build exit `10->CMPLR_FAIL`, `20->LINK_FAIL`, else `BUILD_PREREQ`; run exit `0->PASS`,
`41->RUN_FAIL`, `42->CRASH`, else `RUN_PREREQ`.

---

## 6. GPU (ACE) backend

The same matrix runs on GPU by swapping the builder:

```bash
./tool/run_test.sh --lib ace i64_o10 -o gpu_ace_16k
```

Requirements over the CPU flow:
- `fhe_cmplr` built with `-DFHE_ENABLE_ACE=ON` (default `OFF`), producing `libFHErt_ace.a` and
  the ACE staging/build dirs. `detect_env.sh` auto-probes the GPU section; if absent it skips it
  and the CPU flow is unaffected.
- CUDA toolkit (`nvcc`) and a supported GPU (sm_80/sm_89, e.g. A100, RTX 4090).
- GPU optsets (`gpu_ace_16k`, `gpu_ace_32k`, `gpu_ace_lenet_64k`, `gpu_ace_resnet_64k`) carry
  `-P2C:lib=ace` and an explicit `-CKKS:N=`; you must NOT add `-P2C:df=` (the ACE rtlib has no
  offline encoding) and the bootstrap models need `N=65536` + `:sbm` on an A100.

Full GPU gotchas and `env.sh` GPU variables: `cti`'s `doc/gpu_tutorial.md`.

---

## 7. Failure triage

| Exit / result | Look at | Likely cause |
|---|---|---|
| `BUILD_PREREQ` (30) | script stderr | `tool/env.sh` missing/wrong; input file missing |
| `CMPLR_FAIL` (10) | `run/<case>.<optset>.build.log` | `fhe_cmplr` rejected an op/node or a bad option string |
| `LINK_FAIL` (20) | same build.log, tail | rtlib include/lib paths wrong in `env.sh` |
| `RUN_FAIL` (41) | `run/<case>.<optset>.run.log` | accuracy mismatch; re-run with `ABS_ERROR`/`REL_ERROR`/`PRINT_ALL=1` via `fhe_run.sh`. Expected outputs use CKKS semantics: `onnx2c.py` rewrites MaxPool to AveragePool; test pure ONNX semantics with `--strict-onnx-semantics` |
| `CRASH` (42) | run.log tail | runtime crash (segfault, OOM, ring too small) |
| `GEN_FAIL` | rerun `./tool/gen_onnx_and_main.sh <case>` | missing torch/onnx/onnxruntime on this host |

Independent re-run without rebuilding (the build/run split matters here):
```
ABS_ERROR=0.01 REL_ERROR=0.01 PRINT_ALL=1 ./tool/fhe_run.sh run/i64_o10.native.out
```

Standalone primitive debugging:
```
./tool/fhe_build.sh i64_o10 native operator/onnx/i64_o10.onnx operator/main_file/i64_o10.c "" run/
./tool/fhe_run.sh run/i64_o10.native.out
```

---

## 8. Read-only `cti` checkout fallback

When the `cti` mount is **read-only**, `run_test.sh` cannot write `run/`, `operator/onnx/`, or
`operator/main_file/` in place. The workaround is to copy `local_test` to a writable directory
and run there against the same compiler build (no rebuild needed -- `detect_env` rebinds to the
same `${FHEROOT}`):

```bash
cp -r ${FHEROOT}/cti/local_test ${FHE_LOCAL_TEST_TMP}
cd ${FHE_LOCAL_TEST_TMP}
./tool/detect_env.sh --root ${FHEROOT} --force     # writes ${FHE_LOCAL_TEST_TMP}/tool/env.sh
./tool/detect_env.sh --check
./tool/run_test.sh i64_o10 -o native
```

`${FHE_LOCAL_TEST_TMP}` is any writable directory you choose to hold the copy. In a normal
writable checkout (the dev container, or any machine where `cti` is a full clone), run in
place under `${FHEROOT}/cti/local_test` instead.

---

## 9. Context: `fhe_cmplr` side-effects that local_test isolates

`fhe_cmplr` writes `<base>.conf` (CKKS context JSON) and `<base>.t` (IR trace) to its **process
CWD**, not next to the `-o` output. `cti`'s `fhe_build.sh` runs `fhe_cmplr` with CWD set to the
per-case `run/` out-dir (inputs are absolutized first), so these byproducts stay with the case
and never pollute the source tree. This is the same concern addressed by
`test/run_fhe_cmplr_tmp.sh` in this repo (which `cd`s into a fresh tempdir for ad-hoc runs).

---

## 10. Claude integration: hook vs skill (decision)

The team asked whether the connection should ship as a Claude **hook** or a **skill** in this
repo's `.claude/`. Decision: **ship it as a skill** (`/.claude/skills/local-test/SKILL.md`); do
not add an auto-run hook.

Why a skill:
- Running the regression is an **on-demand developer action** ("I changed the compiler; run the
  operator smoke") and **parametric** (case/group/optset/backend). Skills are invoked
  deliberately with arguments; that matches the task. This repo already uses skills (there
  are two under `.claude/skills/`), so it is the established extension point and teammates know
  the pattern.
- A skill body is procedural knowledge: detect the build root, bind with `detect_env --root`,
  `--check`, smoke, widen. That is exactly what a skill encodes.

Why not a hook as the primary vehicle:
- This repo's `.claude/settings.local.json` is permissions-only; **no hooks are configured**.
  Introducing one changes every teammate's environment (outward-facing) and is a higher bar.
- Hooks fire on **Claude tool events**, not filesystem/git events. A "run tests after build"
  hook would only fire when the build itself is a Claude Bash invocation, matched on command
  text. That is brittle and runs heavy, slow FHE work (a model run is minutes) as a side-effect
  on every matching build. Auto-firing a 30-minute ResNet run is the wrong default.

Optional, low-risk hook (not implemented by default): a `Stop` hook could print a one-line
nudge ("changed the compiler? run `/local-test i64_o10 -o native` to smoke it") without running
anything. If wanted later, add it to `settings.local.json` under `hooks.Stop`. The heavy lifting
stays in the skill, invoked on demand.

---

## 11. Pointers

- `cti` framework reference: `cti/local_test/README.md`
- `cti` usage guide (long-form): `cti/local_test/doc/usage_guide.md`
- `cti` design/milestone docs (operator cases, runner framework, ACE integration, adding a test
  case)
- GPU: `cti` `doc/gpu_tutorial.md`
- This repo's compile-only flow: `test/test_fhe_cmplr.sh`, `test/run_fhe_cmplr_tmp.sh`
- This repo's build: `doc/BUILD.md`, `doc/SETUP.md`
- Claude skill: `.claude/skills/local-test/SKILL.md`
