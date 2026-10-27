# rt_openfhe (python) provider design -- lazy level bridge, bootstrap, data-file, FLEX

## Purpose

`rt_openfhe` is a real-HE rtlib-py provider that executes the emitted
`-CKKS2PY` IR module against real CKKS ciphertexts under OpenFHE 1.5.1 via the
upstream `openfhe-python` pybind11 binding (a ready upstream library the
refactor reuses; not one it writes). It is selected by the single compiler
field `POLY2PY_CONFIG::_py_rt_module`: `-CKKS2PY:rt=rt_openfhe` makes the
emitter write `from rtlib_py.rt_openfhe import *` and the driver load this
provider. It is host-only (no device binding). The refactor reproduces the
feat 2859787f logic: after F2.5 the staged union reproduces the backup's
logic byte-for-byte (`git diff 2859787f -- rt_openfhe/__init__.py` is empty
when run from the v2 worktree).

## Lazy bridge

The lazy bridge stores no tracked `ir_level`. `Cipher.level` is an on-demand
`@property` that recomputes the IR remaining level from the real ciphertext's
consumed depth:

```
Cipher.level = _ctx_ir_capacity() - ct.GetLevel()
```

There is no `level` slot in `Cipher.__slots__` (only `_ct`, `_slots`,
`scale_deg`, the 3-component cipher flag, and `_n_logical`). The single
conversion basis is
`_ctx_num_primes() = _CC.GetMultiplicativeDepth() + 1` -- the REAL built chain
(line 317). For a 3a leveld-SHE context, `GetMultiplicativeDepth() ==
mul_depth`. For a 3b bootstrap context, `GetMultiplicativeDepth() == mul_depth
+ GetBootstrapDepth(...)` -- the real chain is augmented with the bootstrap
reserve, so the post-bootstrap landing never sits deeper than the chain
(`_ctx_num_primes() - ct.GetLevel() >= 0` always). The IR-mapping capacity
`_ctx_ir_capacity()` (line 339) is the chain WITHOUT the R down-headroom
reserve (== `_ctx_num_primes()` only for 3a / R==0); every query that maps an
IR level to an OpenFHE level -- `level(a)`, `_of_level(lv)`, the
encode/raise_mod caps -- uses `_ctx_ir_capacity()`, not the emitted
`input_level` field.

`_of_level(ir_level) = _ctx_ir_capacity() - ir_level` (line 657) converts IR
remaining level to OpenFHE consumed depth (the lazy bridge's single boundary
crossing). Internal ops (`_align`, `_drop_to_of_level`) stay in OpenFHE's
own GetLevel units. Arithmetic ops build results via
`Cipher._wrap(ct, scale_deg, three_comp_flag, n_logical)` (line 594) with no
level argument (the third arg is the 3-component cipher flag, post-`EvalMultNoRelin`
pre-Relinearize); the result level is recomputed on demand from
`ct.GetLevel()`, so a stored tracked level can never drift from the real cipher
level. This is the
structural elimination of the droplevel-upward drift the earlier eager bridge
produced (it stored a tracked level and pinned it per op, so a post-bootstrap
track-only droplevel pinned a higher IR level while the cipher sat at a lower
one; the next `_align` then computed an unreachable upward drop).

`_ensure_of` binds the openfhe binding lazily via `import openfhe as _of`
(line 884); module import is crash-free without the binding (the F1.2
contract stub replaces the binding at test time). The import topology is set
so `from rtlib_py.rt_openfhe import *` does NOT require the binding to be
present (it is fetched at the first real-HE op). The F2.1 design reproduces
the feat 2859787f lazy bridge as observed: `Cipher.__slots__` has no `level`
(line 461); the `level` @property at line 521; `Cipher._wrap` (no level arg)
at line 594; `_ctx_num_primes` at line 317; `_ctx_ir_capacity` at line 339;
`_of_level` at line 641; lazy `import openfhe as _of` at line 884; the `_SK`
lazy-decrypt reference at line 116.

## Bootstrap context + R-tower

`_build_bootstrap_context(of, ctx)` (line 926) builds the model-driven
EvalBootstrap context: it selects the scaling technique via
`_scaling_technique(of, _BTS_SCALING)` (line 1000), sets the secret-key
distribution + correction factor (`SPARSE_TERNARY` + `correctionFactor=13`
under FLEX, `UNIFORM_TERNARY` otherwise), picks `level_budget = [4,4]` (env
`FHE_BTS_LEVEL_BUDGET`), computes the OpenFHE-native
`_bts_depth = FHECKKSRNS.GetBootstrapDepth(level_budget, secret_key_dist)`
(line 1003), and then tunes the IR and chain boundary:

```
_CTX_IR_CAPACITY = max(input_level, max_target + _bts_depth + 1)   # line 1040
R               = max(_BTS_RESERVE, 2)                              # line 1042
depth           = (ir_capacity - _bts_depth - 1) + _bts_depth + R
```

`max_target` is the deepest ReSBM bootstrap target (line 1023: the C
model's app portion `mul_level - Mul_depth_of_bootstrap`, mirroring
`core/src/scheme_info.cxx`). The `+1` margin keeps the deepest target's
droplevel off the exact boundary. The R reserve lives below IR 0 (lines
134-142): R gives IR 1 three towers (1 margin over the non-StCFirst 2-tower
floor; the ModRaise-first path does NOT need the StCFirst lvlb+2 floor).
`FHE_BTS_RESERVE` overrides R (backward compat). Below IR 0 the .py bottoms
out at IR 1 = GL `ir_capacity - 1`; only `EvalBootstrap`'s post-output
landing reaches into the R reserve.

`_ctx_ir_capacity()` stays the IR-mapping ceiling -- IR levels
1..input_level map to the SAME GL positions as a non-bootstrap context -- so
the .py emission does not change between 3a and 3b. The absorbed
level-reduction (origin 992cae93) lives here: the depth formula above
replaces the old `levels_after = mul_depth` double-counting that left the
top ~25 towers idle; the R determination drops the old lvlb+2 guess (the
non-StCFirst path needs ~2 towers, not the StCFirst 6-8).

`Cipher.__init__` is the input-seam: it consumes the IR `level`
(= `_ctx_input_level()` = `mul_depth+1`) to pick the openfhe encryption level
`_of_level(level)` (line 516: `_encrypt_values(padded, _of_level(level),
scale_deg)`). The C-side `Prepare_input` analog (encrypting the seeded input
at the right level via the C-side `Get_encode_scheme(DATA_SCHEME)` routing)
lands at F3.6 -- the Python provider has no `Prepare_input`; the encryption
IS the `Cipher(values, input_level, 1, False)` callsite itself.

## bootstrap_ciph

`bootstrap_ciph(a, level, slot)` (line 1466) is the real-HE bootstrap method.
The flow is:

1. `ct = _CC.EvalBootstrap(a._ct)` -- refresh up to near-top (the
   post-bootstrap landing `post_gl` is `EvalBootstrap`'s level-budget
   result, NOT always openfhe level 0).
2. ant-faithful nsd reset (lines 1542-1548): the binding leaves the real
   `noiseScaleDeg` at 2; the provider forces it back to the canonical
   tracked `scale_deg=1` the emitted schedule expects:
   - FIXEDMANUAL (default, `_FLEX_MODE == False`):
     `while int(ct.GetNoiseScaleDeg()) > 1: ct = _CC.Rescale(ct)` -- the
     real-nsd loop mirrors ant's `Rescale_ciphertext` callsite (ant Rescale
     IS a tower drop, not metadata-only), one tower per excess nsd.
   - FLEX (`_FLEX_MODE`): `for _ in range(_post_nsd - 1): ct =
     _CC.Rescale(ct)` -- the bounded form drops EXACTLY `(post_nsd - 1)`
     towers (same count as FIXEDMANUAL's reset) so FLEX lands at the SAME
     GetLevel as FIXED. The `while nsd>1` form would not terminate under
     FLEX (Rescale does NOT decrement `noiseScaleDeg` under FLEX); the
     bounded form is mandatory.
3. `_drop_to_of_level(ct, target_gl=_of_level(lv), nsd, op)` (line 1559) --
   drop towers to the IR target `lv` when reachable (the lazy-bridge
   teardown). The drop is only issued when `target_gl >= post_gl` (a target
   above the post-bootstrap landing is left as-is -- the lazy bridge has no
   tracked level to pin; the on-demand `level` property reflects the actual
   `post_gl`, so the droplevel-upward drift the eager bridge produced is
   structurally absent).
4. `Cipher._wrap(ct, int(ct.GetNoiseScaleDeg()), False, a._n_logical)` --
   wraps with the 3-component flag cleared to `False` (a bootstrapped cipher
   is relined) and `scale_deg` tracking the REAL post-reset nsd; the result's
   `level` is on demand.

The `slot` argument is the IR ATTR::SLOT linearization slot count
(informational -- the bootstrap slot count is fixed at Setup time,
ring_dim//2). The result's `scale_deg` is the real post-reset nsd (1 FIXED;
2 FLEX), tracked REAL -- not a hardcoded `1`. A non-cipher operand, a
negative target level, or a 3a (no-bootstrap) context raises `[hard]`.

## Data-file (df)

The data-file (df) path is the offline-encode route for `-CKKS2PY:df=`. Three
provider-level functions compose it:

- `_seed_data_info(info)` (line 293): seeds the offline data-file descriptor
  from the emitted module's `get_rt_data_info()`; caches the raw bytes in
  `_DATA_FILE_CACHE` (cleared by `_reset`).
- `_read_data_file()` (line 1686): reads and validates the C binary side
  data file. The header carries the `!ANTFHE` magic at offset 0 (line
  1700), the entry count at offset 16, the LUT offset at offset 24.
  Empty/unseeded/bad-magic paths raise `[hard]`.
- `load_data_entry(idx, n)` (line 1706): reads entry `idx` and returns its
  first `n` message-slot floats. The body mirrors the provider-agnostic
  parser of `rt_ref` and the C `rt_data_file.c`:
  - Reads the entry `sizeof` and `offset` from the LUT (line 1724; the
    offset is a 48-bit bitfield).
  - Validates `n > count` and raises `[hard]` if `n > count` (line 1731 --
    the comment: "the old `slots[:n]` slice would have silently clamped"),
    removing the silent-clamp bug that masks a C-side `count >= len`
    assert.
  - Validates `4096 <= ent_ofst < lut_ofst` and `ent_ofst + size <=
    len(raw)` (line 1737 -- a tampered LUT bitfield would otherwise decode
    header/LUT bytes as floats).
  - Returns `slots[:n]` (truncates to the requested slice; the
    `count >= len` check ensures the slice does not silently over-pad with
    zeros).

The provider's offline path is `DE_MSG_F32`-only (float32). The C-side
`Get_encode_scheme(DATA_SCHEME)` routing -- selecting the encode scheme per
data-file entry from the IR metadata, not a naive contiguous slice -- and the
`sharding-emit = 0` fix live on the C-side handler (F3.6); the Python
provider mirrors the parser and the parity protections (the
`count >= len` check, the LUT-bounds check, the magic check) but does NOT
implement scheme routing. The Python path uses the emitted `input_level`
directly for the input-seam encryption level via `_of_level`.

## FLEX scaling

`FHE_BTS_SCALING` (env, default `fixedmanual`) labels three scaling regimes:

- `fixed` / `manual` / empty (default FIXEDMANUAL, baseline)
- `flex` / `flexibleauto` / `noext` / `flexnoext` (FLEXIBLEAUTO, no EXT
  tower)
- `flexibleautoext` / `fext` / `flexext` / `ext` (FLEXIBLEAUTOEXT, EXT
  tower)

`_parse_flex_scaling(env_val, default)` (line 195) maps an env value to a
tag in `{flexext, flexnoext, fixed}`. `_scaling_technique(of, name)` (line
224) maps the same input to the OpenFHE `ScalingTechnique` enum
(`of.ScalingTechnique.FIXEDMANUAL / FLEXIBLEAUTO / FLEXIBLEAUTOEXT`) for the
`params.SetScalingTechnique(...)` call (line 1000). The two functions agree
on every input (the FLEX shim gates match the built technique). Unknown
values fall back to FIXEDMANUAL with a stderr notice (baseline preserved).

The shim drives three module globals now:

- `_FLEX_MODE` (line 189): True under ANY non-FIXEDMANUAL technique
  (FLEXIBLEAUTO or FLEXIBLEAUTOEXT). The constant-nsd tracking and the
  bounded `(post_nsd - 1)` Rescale reset apply to both -- the binding treats
  every non-FIXEDMANUAL technique identically in Rescale/ModReduce (the real
  `noiseScaleDeg` stays at 2; FLEX auto-manages the real scale).
- `_FLEX_EXT` (line 190): True ONLY under FLEXIBLEAUTOEXT. The `+1`
  Compress compensation is EXT-specific; FIX/FLEX-no-EXT `Compress` is
  exact (no extra tower).
- `_FLEX_SCALE` (line 191): the constant `noiseScaleDeg` under FLEX (= 2).
  The Cipher `scale_deg` clamp at lines 484 and 557 --
  `if _FLEX_MODE and self.scale_deg != 0: self.scale_deg = _FLEX_SCALE` --
  tracks the constant for every non-zero cipher.

The FLEX-EXT Compress-compensation shim lives in `_drop_to_of_level` (line
734):

```
compress_towers = towers_left + 1 if _FLEX_EXT else towers_left
return _CC.Compress(ct, compress_towers, noiseScaleDeg=int(nsd))
```

FLEXIBLEAUTOEXT's `Compress` drops an extra tower; the `+1` keeps the
resulting GL aligned with FIX's post-Compress level (level-budget parity).
The shim is OPT-IN -- it engages ONLY under `FHE_BTS_SCALING=flex*`
(host-only, off-by-default). FIX keeps `_FLEX_MODE = _FLEX_EXT = False`,
so the clamp is a no-op (`self.scale_deg` is unchanged for non-zero
ciphers), the bounded `(post_nsd - 1)` Rescale loop degenerates to the
`while nsd>1` real-nsd loop (the bounded form is identical when
`post_nsd == 2`), and the `compress_towers = towers_left` branch is exact --
the default fixedmanual path is byte-identical to the fixed baseline.

The shim does NOT claim the FIXEDMANUAL scale-drift issue is solved. FLEX
re-anchors the OpenFHE scale per Rescale (FLEX's `AdjustCiphertext` calls
`SetScalingFactor` + the correction multiplier) where FIX divides by the
real prime and `AdjustCiphertext` does not re-anchor; the trade-off is a
slower bootstrap path (FLEX does additional level-alignment work inside
EvalBootstrap). The shim is a separate re-anchor approach for a specific
model class -- the FIXEDMANUAL drift itself remains open. The
`_scaling_technique` selector that introduces FLEX lands at F2.3a with the
parser; the `compress_towers` branch and the `scale_deg` clamp land at
F2.5 as the late opt-in patch. The default baseline keeps FIX, so an
unconcerned workload sees no behavioral change.

## Staging (F2.1 leads, F2.5 closes)

F2 is the highest-risk functional PR (the real OpenFHE python binding, the
lazy bridge, bootstrap, df, and FLEX scaling; the ~50 FLEX refs interleave
the provider per the plan sec.2.2). The staging keeps each landed slice
additive-only and gates the design before any code lands (per plan sec.2.1
gate 1):

- **F2.1** -- this design doc (committed design doc; names the load-bearing
  elements before any code lands).
- **F2.2** -- OpenFHE EvalBootstrap toy POC (a standalone python script
  mirroring F4.2, exercising `EvalBootstrap` + `Rescale` +
  `LevelReduceInPlace` on a toy CKKS ring; gap-filler -- proves the
  openfhe-python binding mechanism recovers a message within a 1e-3
  tolerance before the provider lands).
- **F2.3a** -- core context + lazy bridge + `Cipher`/`Plain` + shared
  helpers + the FLEX parser (`_parse_flex_scaling`,
  `_scaling_technique`, `_FLEX_MODE`/`_FLEX_EXT`/`_FLEX_SCALE`) + the FULL
  dispatcher forward-refs (additive to `rt_openfhe/__init__.py`,
  hand-re-authored). The FLEX globals are declared but their gated
  branches land as no-ops until F2.5.
- **F2.3b** -- arithmetic ops (incl. `raise_mod`) + encode + IO + the df
  offline-encode path (`_seed_data_info`/`_read_data_file`/`load_data_entry`)
  + query/validate/timing (additive).
- **F2.4** -- bootstrap context (`_build_bootstrap_context`) +
  `bootstrap_ciph`; absorbs the level-reduction tuning
  (`ir_capacity = max(input_level, max_target + bts_depth + 1)`,
  `R = max(_BTS_RESERVE, 2)`).
- **F2.5** -- the FLEX-EXT Compress-compensation patch: the
  `compress_towers = towers_left + 1 if _FLEX_EXT else towers_left` branch
  inside `_drop_to_of_level` + the `Cipher.__init__` FLEX `scale_deg`
  clamp. Late, opt-in, labeled. After F2.5,
  `git diff 2859787f -- rt_openfhe/__init__.py` is empty -- the staged union
  reproduces the backup's logic exactly.

F2.6 (tests) follows and is not part of this design doc.

## References

  * PR676 `doc/pr676-refactor/REFACTOR-PLAN.md` -- section 3 F2 (stage
    table); section 2.2 (incremental construction of the monolithic
    `rt_openfhe/__init__.py`); section 2.1 gate 1 (design-first gate this
    doc satisfies).
  * `doc/rtlib-py-doc/fhert-backend-design.md` -- the F4.1 parallel
    monolithic provider design; this doc mirrors its structure and the
    committed-design-doc pattern.
  * `doc/rtlib-c-bootstrap/openfhe-bootstrap-integration-plan.md` -- the
    C-side F3.1 mirror of the model-driven `EvalBootstrap` context
    (`ir_capacity`/`R`/`bts_depth` sizing) this provider ports to python.
  * `rtlib-py/rtlib_py/rt_openfhe/__init__.py` (feat backup `2859787f`):
    `Cipher.__slots__` (line 461), `level` @property (line 521),
    `Cipher._wrap` (line 594), `_ctx_num_primes` (line 317),
    `_ctx_ir_capacity` (line 339), `_of_level` (line 641), lazy
    `import openfhe as _of` (line 884), `_build_bootstrap_context`
    (line 926), `bootstrap_ciph` (line 1466), `_drop_to_of_level`
    (line 734), `_seed_data_info` (line 293), `_read_data_file`
    (line 1686), `load_data_entry` (line 1706). Line numbers refer to the
    feat backup `2859787f`.
