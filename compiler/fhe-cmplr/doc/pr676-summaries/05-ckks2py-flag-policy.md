# PR #676 - Part 5: ckks2py flag-policy end-state + cleanup

This is the fifth of five functional parts that compose PR #676
(`feat/openfhe-c-bootstrap`), landed as reviewable commits on the
`feat/openfhe-c-bootstrap-v2` branch (off `master`, base commit `30f387ff`).
Because the review system accepts only one overall PR, each functional part ends
with a marker commit that carries a summary like this one - the per-part
description that would otherwise live in a separate PR message. This commit is
Part 5's marker.

## Scope

- **F5** - the ckks2py **flag-policy end-state and cleanup**: the END STATE of
  the `-CKKS2PY` pass flag policy, the rename-narrative residue cleanup, the
  SKIP-gate finalize at the run_py_exec executor, the result-dump purge and
  CJK/ASCII-style/English cleanup pass on kept methodology, the rtlib-py-doc
  sync to the end state, and the ASCII hygiene pass that closes the PR. The flag
  policy is set natively from the first commit (no rename step, no rename
  narrative); Part 5 only writes down the end state and scrubs residue.

The four prior functional parts already landed: the foundation + rtlib-py
framework + `rt_ref` (Part 1), `rt_openfhe` (Part 2), the OpenFHE C-side
bootstrap (Part 3), and `rt_fhert` host+device (Part 4). Part 5 is the
closing part; nothing else ships in this PR.

## The flag-policy END STATE

The `-CKKS2PY` group's flag policy at the end of this PR:

- **`-CKKS2PY:rt=<module>` is the SOLE activator.** Set at `ckks2py_pass.cxx`
  and gated by `ckks2py_config.h` `Py_rt_empty()`. With `rt=rt_ref` the emitter
  emits `<model>.onnx.py` with `from rtlib_py.<module> import *` and no `.c`
  leak. With no `-CKKS2PY` flag the default C path emits `<model>.onnx.c` (no
  `.py`); the emitted C is md5-stable across clean reruns.
- **`-CKKS2PY:lib=` is REMOVED.** It is absent from the `Ckks2py_option[]`
  table in `ckks2py_config.cxx`, so the air parser rejects it at parse-time
  with `Fatal error: Incorrect option, lib=<value>` (exit 1 - a strong
  rejection, not a silent ignore). It rejects alone AND when combined with a
  valid `rt=` value.
- **`-CKKS2PY:fp` inserts `free_ciph(...)` calls.** With `rt=rt_ref -CKKS2PY:fp`
  the emitted `.py` carries `free_ciph(...)` at the model tail (lines 16-17 in
  the add.onnx probe); without `-CKKS2PY:fp` there are zero `free_ciph(` call
  sites.
- **Dead `_prov_str` plumbing never lands.** The python path uses only the
  `POLY2C_CONFIG` base member that the C `POLY2C` path consumes. The
  `_prov_str` symbol is absent from the emitter headers (no mirrored /
  no-longer-used variants added and removed; the dead plumbing never lands).
- **Headers at `include/fhe/ckks/` (`fhe::ckks` namespace).** The
  ckks-to-python emitter headers live at this path from the first commit
  (F0.2 in Part 1 sets the layout; Part 5 only refines comments).
- **NO rename step / NO rename narrative.** The PR uses `-CKKS2PY` and
  `fhe::ckks` natively from the first commit. There is no `poly`-to-`ckks`
  rename step to schedule or roll back, and no "step 2" narrative in the kept
  comments.

## What it adds

- **F5.1 - end-state design note** (`doc/ckks2py-flag-policy.md`, net-new) the
  END-STATE design note for the `-CKKS2PY` flag policy. States rt= is the sole
  activator; lib= is removed; fp inserts free_ciph; the headers live at the
  `include/fhe/ckks/` (fhe::ckks) location. Inspected the v2 headers; the
  option table and activation flow were already at end-state, with only stale
  rename-narrative (lib= / step 2 / `_prov_str`) comment residue left for F5.2.
  Pure design note - no headers, no impl.
- **F5.2 - end-state impl (comment cleanup, runtime-verified semantics)**
  the five `include/fhe/ckks/` and `ckks/src/` files
  (`ckks2py_pass.h`, `ckks2py_config.h`, `ir2py_ctx.h`; `ckks2py_pass.cxx`,
  `ckks2py_config.cxx`) have their rename-narrative comments replaced with
  end-state prose on `rt=`. The phrases "was removed in step 2", "the
  -CKKS2PY:lib= option", "non-ant -CKKS2PY:lib=", and the `_prov_str`
  no-longer-used / mirrors variants are gone - comment-only; no live code
  change. The end state was verified at runtime from a scratch dir on a small
  op: (a) `rt=rt_ref` emits `.py` with the rt_ref import and no `.c`; (b)
  `lib=foo` (alone and combined with `rt=rt_ref`) is rejected with exit 1
  "Incorrect option, lib=foo"; (c) `rt=rt_ref -CKKS2PY:fp` emits `free_ciph`
  at the tail; (d) no `-CKKS2PY` flag emits the C path with md5-stable output.
  The default allops ctest path emits rt_ref and is unaffected.
- **F5.3 - run_py_exec SKIP gate** a real-HE provider (`rt_openfhe` /
  `rt_fhert`) whose upstream binding is not importable now exits 77 (ctest
  SKIP_RETURN_CODE) instead of crashing the executor with an uncaught
  `ImportError` from the lazy `_build_context` import (driver.run does not
  wrap `prov._seed_ctx`). `rt_ref` needs no probe. Adds SKIP_RETURN_CODE 77 +
  TIMEOUT 300 to the `test_ckks2py_allops` ctest so a binding-absent /
  harness-absent env SKIPs rather than FAILs. The
  C++ harness already returns 0 on install-only; the executor now reserves
  77 for a missing provider binding. The default allops path (emits rt_ref)
  is unaffected.
- **F5.4 - purge + CJK / style / English cleanup pass** the result-dump
  inventory is confirmed empty (no result-dump fixtures -- the OUT'd
  e2e-py-results / fhe-cgo-accuracy / sweep-onnx / runner-schema artifacts --
  in tracked files; no dangling refs to the OUT'd runner schema in kept
  docs). CJK scan on the
  PR676-modified files is 0 - the lone CJK in the tree is `rt_phantom.h`, whose
  blob is byte-identical between master `30f387ff` and HEAD and is OUT of PR676
  scope. The hardcoded build-path defaults in the py_verify runners
  become `FHE_CMPLR` env / flag only (no host-build fallback); the
  `run_resnet20_vs_onnx.py` mirror-check rejects an empty default cleanly.
  Section-6.8 residue grep hits 0 on PR676-modified files after trimming the
  dangling stage-doc refs in `rtlib/openfhe/src/openfhe_lib.cxx` and
  `rtlib/openfhe/unittest/ut_openfhe_bootstrap.cxx` (the inline technical
  rationale is kept). The prfix `8e1011cc` snapshot is treated as REFERENCE
  only, not cherry-picked.
- **F5.5 - sync rtlib-py-doc to flag-policy end-state** brings 00-README,
  10-new-backend-integration, 20-implementation-status as PR676
  section-5-trimmed new files (em-dash to `--`, drop host-build path, drop
  step-2 rename narrative, drop max_abs literal); each carries the
  flag-policy paragraph (rt= is the sole activator; lib= is removed;
  reference `doc/ckks2py-flag-policy.md`). Brings 30-test-status and
  50-resnet-benchmark-case-study as the section-5 trim target (the 30
  appendix anchor renamed to non-regress-results with the 40 evidence
  links updated). 60-development-history-and-review-guide is OUT (section 5
  OUT). 40-open-issues carries the same flag-policy paragraph at the intro
  and the 5 evidence-anchor refs are updated.
- **F5.5b - ASCII hygiene across the PR676-modified tree** the per-stage
  ASCII gates earlier in this PR only checked for em-dash, en-dash, and CJK
  byte classes; broader non-ASCII slipped through - em-dashes (36),
  section signs (48, in section refs), and 1 right-arrow - across the
  01/02 marker-file bodies, some byte-verbatim `rt_openfhe` docstrings,
  and a few F4/F5 comment lines
  (`common/timing.py` / `rt_ref` / `py_verify` / ckks2py headers). Replaced
  the non-ASCII byte sequences in comments / docstrings / prose only:
  em-dash to `--`, section-sign to `section`, right-arrow to `->`. No
  executable code or test golden strings changed; no string-data non-ASCII
  required preserving. `03-openfhe-c-bootstrap.md` was already clean and left
  untouched.

## Gates

- **rt= activates** - `rt=rt_ref` emits `.py` with the rt_ref import and no
  `.c`; default (no `-CKKS2PY`) emits the C path; the emitted C is md5-stable
  across clean reruns.
- **lib= rejected** - `-CKKS2PY:lib=foo` exits 1 with "Incorrect option,
  lib=foo" (alone AND combined with `rt=rt_ref`).
- **fp -> free_ciph** - `-CKKS2PY:rt=rt_ref -CKKS2PY:fp` emits `free_ciph`
  at the model tail; zero `free_ciph(` call sites without `-fp`.
- **`-P2P` absent** - 0 hits in kept `.h` / `.cxx` / `.cc` / `.md` (a .py
  sweep driver script that retains the token is OUT of F5.2 scope).
- **no CJK in kept files** - PR676-modified files CJK scan is 0; the lone
  tree CJK is `rt_phantom.h` (master-era, out of scope).
- **no result CSV / JSON in kept dirs** - OUT result-dumps 0 in tracked
  files; no dangling refs to the OUT'd runner schema.
- **section-6.8 residue 0** - stage-doc-ref grep 0 on PR676-modified files
  after the F5.4 trims.
- **ASCII-clean across the PR676-modified tree** - em-dash / en-dash / CJK /
  section-sign / right-arrow / box-drawing / math / smart-quotes scan 0.
- **g++ -fsyntax-only on the ckks2py headers** - green; the emitter headers
  compile cleanly.
- **py_compile on kept .py files** - all kept PR676-modified .py files
  py_compile clean.
- **rt_fhert byte-identity** - `git diff 2859787f HEAD --
  rtlib-py/rtlib_py/rt_fhert/__init__.py` shows only the F4 docstring
  section-5 trim; F5.5b did not touch `rt_fhert/__init__.py`.

## Honest follow-ups (limitations, not silent "done")

- **check_coding_style CI target NOT wired in this build env.** The function
  lives in `PACKAGE_DEVTOOLS_DIR`, which is empty here; `gmake
  check_coding_style` returns rc=2 (`No rule to make target
  'check_coding_style'`). The F5.2 edits are pure prose comments with no style
  impact, so the proxy is `-fsyntax-only`: `g++ -std=c++17 -fsyntax-only`
  green on the ckks2py headers. The check_coding_style run is recorded as a
  follow-up for an env with `PACKAGE_DEVTOOLS_DIR` populated.
- **Section-2.6 sanctioned deviations on the road to the end state.** The
  following PR676 tree edits are comment / docstring / docs-only and were
  recorded as sanctioned deviations under section 2.6:
  - the `rt_fhert/__init__.py` module-docstring section-5 trim (F4.4, F4.8 -
    the `cp3.12` / `cp3.13` host-build-mismatch narrative excised); code
    logic unchanged.
  - the `rt_openfhe` / `rt_ref` / `common/timing.py` em-dash-comment to
    `--` ASCII-hygiene (F5.5b); comments / docstrings only, code logic
    unchanged.
  - the F3 C-side dangling stage-doc-ref cleanup in
    `rtlib/openfhe/src/openfhe_lib.cxx` and
    `rtlib/openfhe/unittest/ut_openfhe_bootstrap.cxx` (F5.4); the inline
    technical rationale kept, code logic unchanged.
  All three deviations are sanctioned (path-parameterization / OUT-trim /
  CJK-to-English / ASCII-hygiene / dangling-ref removal). The non-comment
  scope of the kept PR676 files is unchanged.
- **Prior-PR marker commit subjects retain em-dash.** The PR-A (`a6f1613d`),
  PR-B (`cafff57a`), and PR-C (`221673de`) marker COMMIT SUBJECTS still
  contain an em-dash in the "Part N" phrase - they are NOT rewritten here.
  Rewriting commit subjects would require rebasing the marker commits and
  re-backing-up the pinned backup refs. The marker FILE BODIES (01 / 02 .md)
  were ASCII-fied at F5.5b; the 03 and 04 marker bodies were already clean.
- **`rt_phantom.h` CJK is OUT of PR676 scope.** A memory note predicted CJK in
  `rtlib/include/rt_phantom/*` (a master-era phantom backend); the F5.5b check
  confirmed the `rt_phantom.h` siblings are actually already ASCII-clean -
  the note was stale. The broader full-repo non-ASCII files
  (`LEGAL.md`, `BUILD.md`, `key_gen.c`) are master-era unchanged and out of
  the PR676 diff. PR676 did not introduce or carry CJK; the CJK in the tree
  predates the PR676 branch base.
