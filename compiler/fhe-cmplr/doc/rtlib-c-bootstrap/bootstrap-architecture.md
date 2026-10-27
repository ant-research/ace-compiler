# F3.9 - OpenFHE C-side bootstrap architecture

A brief methodology note for the C-side bootstrap landed in F3.1-F3.10.
Captures the API surface and the fixed seams so a future reader can navigate
the code without re-deriving them.

## OpenFHE 1.5.1 EvalBootstrap API surface

The C-side bootstrap in `rtlib/openfhe/src/openfhe_lib.cxx` uses four
OpenFHE primitives, in this order:

- `EvalBootstrapSetup(levelBudget, dim1, numSlots)` - builds the bootstrap
  parameters once per context.
- `EvalBootstrapKeyGen(secretKey, numSlots)` - generates the bootstrap key set
  (the heavy lifetime allocation).
- `EvalBootstrap(ciphertext)` - the refresh itself; returns a ciphertext
  near the top of the modulus chain. On OpenFHE 1.5.1 the signature has no
  per-call target level.
- `Rescale(ciphertext)` - applied in a `while GetNoiseScaleDeg() > 1` loop
  after `EvalBootstrap` so the noise-scale-deg comes back to the canonical 1
  the emitted rescale-after-every-mul schedule expects.
- `LevelReduceInPlace(ciphertext, nullptr, levels)` - the scale-preserving
  tower drop that lands the refreshed ciphertext on the IR target level.
  `nullptr` is the no-key-switch overload (matches the feat `Mod_switch`).

## Chain sizing

- `ir_capacity` is the chain length reserved for the bootstrap path. It is
  sized so the bootstrap headroom plus the model's multiplicative depth both
  fit: max(input_level, max_target + bts_depth + 1). The R-tower reserve sits
  below IR 0 so the lowest IRs are reachable after the bootstrap drops down.
- `R` (the extra R tower count) is `max(FHE_BTS_RESERVE, 2)` - the reserve
  floor is 2; `FHE_BTS_RESERVE` can raise it.
- `FHE_BTS_SF` (scale-factor bits, default 59) and `FHE_BTS_RESERVE` are
  env-overridable so a model can tune precision / headroom without a recompile.

## level_budget

`[4, 4]` for the two bootstrap passes (c2s, s2c) over `UNIFORM_TERNARY`
secret + `HYBRID` keyswitch on `FIXEDMANUAL` scaling. Matches the
simple-ckks-bootstrapping example; the resulting `btsDepth` comes from
`FHECKKSRNS::GetBootstrapDepth(levelBudget, secretKeyDist)`.

## Prepare_input input-seam contract

`Prepare_input` encrypts inputs at a fixed seam level so the bootstrap has
room below it:

  `of_level = _bts_ir_capacity - input_level`

and emits one ciphertext per scheme shard per its `MAP_DESC`. The seam is
the contract the compiler side hands the runtime: the compiler's
`Emit_need_bts()` (in `include/fhe/poly/ir2c_ctx.h`) emits `bool Need_bts()`
for `PROVIDER::OPENFHE`, so fhe-cmplr-generated code for `-P2C:lib=openfhe`
links against the bootstrap path.

## Openfhe_copy deep-Clone fix

`Openfhe_copy` must deep-`Clone()`. The shallow `*res = *op` aliased dst with
src, and the in-place `EvalAddInPlace` then corrupted src during the
op/gemm BSGS reduce. The fix replaces the assign with a `Clone()` so dst owns
its own backing towers before any alias-sensitive reduce runs.

## Get_encode_scheme sharding fix

`Prepare_input` shards inputs by `Get_encode_scheme(DATA_SCHEME)`, not by a
contiguous slice. Each scheme shard becomes its own ciphertext at the seam
level, so every shard stays within the slot count. This fixes the
resnet18-conv2d `:sharding` crash where a contiguous slice exceeded the
per-ciphertext slot count.
