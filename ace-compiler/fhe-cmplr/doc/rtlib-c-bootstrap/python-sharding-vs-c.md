# Python rt_openfhe sharded-input handling vs the C openfhe `Prepare_input` gap

Read-only investigation. The C-side OpenFHE backend (`rtlib/openfhe`) fails one
regress58 case, `resnet18-conv2d` (`-VEC:ms=4096:sharding
-CKKS:q0=60:sf=59:N=8192`), because `OPENFHE_CONTEXT::Prepare_input` encodes the
whole input tensor into ONE ciphertext and stores only
`Io_set_input(name, 0, ct)`, while the generated graph reads 4 shards via
`Get_input_data("input", idx)` for idx 0..3 and `Get_encode_scheme` declares
k=4 NORMAL shards of 3136 values. OpenFHE rejects the whole-tensor encode at
`MakeCKKSPackedPlaintext` when the tensor size exceeds the slot count (analysis
in `regress58-sharding-gap-analysis.md`).

This document answers the follow-up: does the PYTHON path (rtlib-py's
`rt_openfhe`) have the same problem, and how does it handle sharded input
compared to the C path.

## 1. Bottom line

- **Does Python rt_openfhe have the problem?** No. The Python path does NOT
  encode the full tensor into one cipher. The rtlib-py driver seeds
  `_INPUTS[(name, k)]` for k = 0..3 (separate ciphers, one per shard, each
  holding the shard's values zero-padded to the slot count), built from the
  compiler-emitted `get_encode_scheme`. The graph's
  `get_input_data("input", idx)` for idx 0..3 finds a seeded cipher at each
  index. No single `MakeCKKSPackedPlaintext` ever receives more values than
  the slot count, so it never hits the overflow throw.
- **Is the Python path a better reference than ant for the C fix?** No. Ant
  (`rtlib/ant/ckks/src/rtlib.c:164-192`) is still the cleaner reference,
  because it lives in the C rtlib next to the buggy
  `OPENFHE_CONTEXT::Prepare_input` and uses the same
  `DATA_SCHEME`/`MAP_DESC`/`Pre_encode_scheme`/`Io_set_input` calls the C fix
  would use. The Python scheme-packing is split across the host-language
  driver (`driver.py`) and the provider (`rt_openfhe/__init__.py`), and its
  slice is a small Python loop, not a rtlib helper. Porting the algorithm is
  straightforward either way, but ant is the match-by-match C analogue.

## 2. Python input-prep mechanism (file:line)

The Python driver `rtlib_py/common/driver.py` is the `Prepare_input`
equivalent. The flow is:

1. The emitted `.py` module defines `get_context_params()`,
   `get_input_count()`, `get_encode_scheme(idx)`, `get_rt_data_info()`, and
   `Main_graph()` (mirroring the C weak symbols). It begins with
   `from rtlib_py.rt_openfhe import *`.
2. `driver.run()` execs the module, seeds `_CTX` from
   `get_context_params()`, seeds the offline data-file descriptor, then
   fetches the input schemes:
   ```
   gges = ns.get("get_encode_scheme")
   gic  = ns.get("get_input_count")
   if gges is not None and gic is not None:
       schemes = {}
       for i in range(int(gic())):
           s = gges(i)
           schemes[s["name"]] = s
   ```
3. `_seed_inputs(prov, spec, schemes)` packs the whole input tensor into
   per-shard ciphers. For the `:sharding` resnet18-conv2d case the emitted
   scheme declares `num_chunk=4` with four `desc` entries (count=3136, starts
   0, 3136, 6272, 9408), so the scheme branch runs:
   ```
   if desc and desc[0].get("count", 0) > 0:
       shape = sch.get("shape") or []
       hw = shape[2] * shape[3] if len(shape) >= 4 else None
       for k, d in enumerate(desc):
           cv = _pack_cipher(v, d, hw)
           cipher = prov.Cipher(cv, input_level, 1, False)
           prov._seed_input(name, k, cipher)
   ```
   `_pack_cipher` slices the flat tensor per the desc: leading neighbor-pad
   (when `pad>0` and `start` is not at a channel boundary), then `count` body
   values at `start + j*stride`, then trailing neighbour-pad, then zero-pad
   to `n_slots`. For resnet18 all four shards have `pad=0, stride=1`, so each
   cipher is exactly `flat[start .. start+count-1]` + zeros to the slot
   count. Each shard's values fit the slots (N=8192 => N/2=4096).
4. `prov._seed_input(name, k, cipher)` stores `_INPUTS[(name, k)] = cipher`.
   The constructor `Cipher(slots, level, scale_deg, False)` encrypts via
   `_encrypt_values(padded, _of_level(level), 1)`, which calls
   `_CC.MakeCKKSPackedPlaintext(padded, 1, of_level, slots=_N_SLOTS)` and
   `_CC.Encrypt(_PK, pt)`. Each of the per-shard ciphers is encrypted
   independently and stored at its own `(name, k)`.
5. At runtime the graph calls `get_input_data("input", idx)`:
   ```
   key = (name, idx)
   if key not in _INPUTS:
       _violation("hard", "get_input_data", f"no input seeded for ({name!r}, {idx})")
   return _INPUTS[key]
   ```
   each shard index resolves.

The degenerate-scheme fallback handles single-cipher / non-tiled inputs (the
compiler emits a placeholder desc `{count:0, start:0, pad:0, stride:0}` when
the IR input node has no `DATA_SCHEME` attribute -- see section 4): it guards
on `desc[0].get("count",0) > 0` and, when false, packs the tensor into
`K = ceil(n / n_slots)` contiguous chunks (K=1 when `n <= n_slots`, a no-op
vs the old single-cipher path). A contiguous chunk never exceeds `n_slots`,
so the fallback too cannot overflow into a single cipher that exceeds N/2
slots.

## 3. Does the Python path have the problem?

No. Two independent guarantees prevent it:

- The scheme branch emits exactly `num_chunk` ciphers, each holding
  `desc[k].count` values, zero-padded to `_N_SLOTS`. Never more than the slot
  count per cipher.
- Even the no-scheme / degenerate fallback chunks contiguously into
  `ceil(n / n_slots)` ciphers, so a large input still becomes multiple
  ciphers, never one.

The encodes that happen are per-shard `MakeCKKSPackedPlaintext(<shard
values>)` at `_of_level(input_level)`, nsd 1, all within ring slots -- the
exact behavior the C fix in `regress58-sharding-gap-analysis.md` section 5
prescribes.

## 4. Side-by-side comparison

| Concern | C openfhe backend | Python rt_openfhe |
|---|---|---|
| Where input tensor -> cipher | `OPENFHE_CONTEXT::Prepare_input` `rtlib/openfhe/src/openfhe_lib.cxx:44-69` | `driver._seed_inputs` `rtlib-py/rtlib_py/common/driver.py:78-156`, ciphers built by `prov.Cipher(...)` `rt_openfhe/__init__.py:364-406` |
| Consults the emitted shard plan | No -- `Prepare_input` ignores `Get_encode_scheme`/`Get_input_count`, encodes the whole tensor | Yes -- fetches `get_encode_scheme`/`get_input_count` and packs per `desc[k]` |
| Sets per-shard idx | Only `Io_set_input(name, 0, ct)` (`openfhe_lib.cxx:69`) | `_seed_input(name, k, cipher)` for k=0..num_chunk-1 |
| Encoder overflow | Yes -- whole-tensor `MakeCKKSPackedPlaintext` throws at `openfhe_lib.cxx:66` when the tensor exceeds the slot count | No -- each per-shard `MakeCKKSPackedPlaintext` fits the slots |
| `get_input_data("input", 1..3)` | Never reached (throw first); would be `nullptr` | Resolves a seeded cipher at each idx |
| Emitted shard plan itself | Correct -- `Get_encode_scheme` in the generated `.c` has the 4 NORMAL shards (gap-analysis section 1c) | Correct -- `get_encode_scheme` in the emitted `.py` has the same 4-shard `desc` (same `nn::core::Data_scheme_attr` source -- section 5) |
| resnet18-conv2d outcome | CRASH (SIGABRT) | PASS (real CKKS, recorded -- section 7) |

## 5. The emitter side: one shared `DATA_SCHEME` source, placeholder when absent

The compiler emits the shard plan for both paths from the SAME function,
`nn::core::Data_scheme_attr(node, &num_chunk)`:

- C emitter `poly/src/poly2c_driver.cxx:124-138` (`Emit_chunk_info`) writes
  `MAP_DESC desc_N[] = {{NORMAL, count, start, pad, stride}, ...}`; when the
  node lacks a `DATA_SCHEME` attribute it writes the placeholder
  `{NORMAL, 0, 0, 0, 0}`.
- Python emitter `ckks/src/ckks2py_driver.cxx:201-220` (`Emit_data_scheme_py`)
  writes the dict `{'kind':'NORMAL','count':N,'start':...,'pad':...,'stride':...}`;
  the same `else` placeholder is
  `{'kind':'NORMAL','count':0,'start':0,'pad':0,'stride':0}`.

Both emitters call the same `Data_scheme_attr`: `poly2c_driver.cxx:127` and
`ckks2py_driver.cxx:205`. For a `:sharding` input (resnet18-conv2d) the IR
node carries a real `DATA_CHUNK[]`, so BOTH the C `.c` and the Python `.py`
carry the correct multi-shard plan (the C plan was confirmed in
`regress58-sharding-gap-analysis.md` section 1c). So the C-side failure is
NOT an emitter bug -- `Get_encode_scheme` is correctly emitted in the `.c`;
the C openfhe `Prepare_input` simply never reads it.

## 6. The Python-side "sharding-emit =0 bug" (related but distinct, already fixed)

The memory note "sharding-emit =0 bug must be fixed too" refers to a DIFFERENT
Python-side bug, not the C single-cipher overflow. It was folded into commit
`d51d4dbf` (`feat(poly2py): add ckks IR->Python backend + rtlib-py runtime`);
there is no standalone fix commit. Two co-fixed bugs:

1. Driver single-idx seeding. Before the fix, `_seed_inputs` only seeded
   `(name, 0)`, so the graph's `get_input_data("input", 1)` raised `[hard]
   get_input_data: no input seeded for ('input', 1)`. The fix is the
   scheme-driven loop in `driver.py` (the scheme-based `_seed_inputs`
   block).
2. Emitter cipher-array zero-init. `ir2py_core.h Handle_st` for a ST of ZERO
   into a cipher-array accumulator (a sharded-conv `VS_out`) emitted the
   scalar `dst = 0`, overwriting the `[None]*N` declaration and crashing the
   later subscript with `TypeError: 'int' object is not subscriptable`. The
   fix adds an array-of-cipher branch (`include/fhe/ckks/ir2py_core.h:245-263`):
   ```
   {
     air::base::TYPE_PTR tp = node->Addr_datum()->Type();
     if (tp->Is_array() && val_opc == air::core::OPC_ZERO &&
         ctx.Is_cipher_type(tp->Cast_to_arr()->Elem_type()->Id())) {
       uint64_t n = tp->Cast_to_arr()->Elem_count();
       ctx.Emit_var(node);
       ctx << " = [zero_ciph() for _ in range(" << n << ")]";
       return RETV();
     }
   }
   ```
   This is the Python analogue of the C `CIPHERTEXT arr[N]` decl (each slot
   a fresh zero cipher so `add_ciph(VS_out[idx], ...)` reads a cipher). The
   C path has no analog of this crash because C declares cipher arrays
   natively.

Evidence of the before/after on the Python resnet18-conv2d run (same
optset):

- Before the fix: the recorded pre-fix runs under both `rt_ref` and
  `rt_openfhe` failed at input seeding (`[hard] get_input_data: no input
  seeded for ('input', 1)` / `_seed_inputs` input-seeding failure).
- After the fix: the recorded post-fix runs PASS for both df-off (real
  CKKS) and df-on.

(The C path also has a compiler-side `Emit_chunk_info` placeholder
`{NORMAL,0,0,0,0}` when `chunk == nullptr`, but that is the degenerate
single-cipher case, not the resnet18 case -- resnet18's chunk is present, so
the placeholder never appears.) The C single-cipher overflow gap
(`Prepare_input` not reading `Get_encode_scheme`) remains unfixed and is
unrelated to the Python-side fixes.

## 7. Empirical confirmation

Compiling the resnet18-conv2d `:sharding` case to the Python rt_openfhe path
with the catalog optset (df-off) and inspecting the emitted module confirms
the sharding mechanism from plain text in the emitted `.py` (no exec of the
large df-off module needed):

1. The emitted `get_encode_scheme` carries the REAL 4-shard `DATA_SCHEME`
   (`num_chunk=4`, `count=3136`, starts 0, 3136, 6272, 9408) -- the same
   content the C `.c` carries per `regress58-sharding-gap-analysis.md`
   section 1c:
   ```
   def get_input_count(): return 1
   def get_encode_scheme(idx):
     schemes = [
       {'name': 'input', 'shape': [1, 16, 28, 28], 'num_chunk': 4, 'desc': [
         {'kind': 'NORMAL', 'count': 3136, 'start': 0,    'pad': 0, 'stride': 1},
         {'kind': 'NORMAL', 'count': 3136, 'start': 3136, 'pad': 0, 'stride': 1},
         {'kind': 'NORMAL', 'count': 3136, 'start': 6272, 'pad': 0, 'stride': 1},
         {'kind': 'NORMAL', 'count': 3136, 'start': 9408, 'pad': 0, 'stride': 1}]},
     ]
     return schemes[idx]
   ```
2. The graph reads exactly these shards:
   ```
   input_0 = [None] * 4
   for input_idx in range(4):
     input_0[input_idx] = get_input_data("input", input_idx)
   ```
3. The context is `poly_degree: 8192` -> N/2 = 4096 slots; each shard's 3136
   values fit (the C bug was the whole-tensor encode exceeding the slot
   count). The sharding-emit fix is live in the emitted module: the conv
   accumulators are `[zero_ciph() for _ in range(4)]` -- the exact text
   emitted by `ir2py_core.h:260`.
4. Driver seeding: the `desc[0].get("count",0) > 0` guard is TRUE for this
   scheme -> the scheme branch packs 4 ciphers, one per shard, each
   `flat[start .. start+count-1]` zero-padded to `_N_SLOTS`, and stores
   `_seed_input("input", k, cipher)` for k=0..3. Each
   `MakeCKKSPackedPlaintext` receives only the shard's values, never the
   whole tensor.

The recorded full-run result for this exact optset is a PASS on real CKKS
(an independent recorded run; the large df-off Python module is documented
as a pathologically slow Python-scalability case, not a sharding defect; the
df-on variant passes identically). The mechanism above is the decisive
evidence and does not depend on re-running.

## 8. Implication for the C fix

Ant remains the better reference for the C `OPENFHE_CONTEXT::Prepare_input`
fix, not the Python path:

- Ant's `Prepare_input` (`rtlib/ant/ckks/src/rtlib.c:164-192`) sits in the
  same C rtlib tree as `OPENFHE_CONTEXT::Prepare_input`, uses the identical
  `DATA_SCHEME`/`MAP_DESC`/`Get_encode_scheme`/`Get_input_count`/`Io_set_input`
  API, and demonstrates the exact `Pre_encode_scheme` slice
  (`rtlib.c:67-110`) the C fix reimplements inline. The fix design in
  `regress58-sharding-gap-analysis.md` section 5 mirrors ant line-for-line.
- The Python scheme-packing is split across the host-language driver
  (`driver.py`) and the provider (`rt_openfhe/__init__.py`), and its slice
  is a Python loop over `desc[k]`; porting it to C would still reimplement
  the same windowed slice ant already provides as `Pre_encode_scheme`. There
  is no rtlib-py helper to reuse -- the Python `_pack_cipher` is pure Python
  in the driver, not shared C code.
- The Python path DOES confirm the algorithm (slice per
  `Get_encode_scheme`/`get_encode_scheme` into k ciphers at the same
  `of_level`, `Io_set_input(name, k, ct)` for k=0..count-1) and is a useful
  INDEPENDENT validation that the scheme-driven approach yields a correct
  result on real CKKS (the archived PASS). But as a code reference for the C
  edit, ant is the direct analogue.

Net: the Python path proves the fix direction is correct and already works
on real CKKS; ant is the C-reference implementation to mirror. No change to
the prior fix design (`regress58-sharding-gap-analysis.md` section 5) is
needed -- it already mirrors ant, which the Python path independently
validates.

## Evidence index (file:line)

- Python driver input seeding (scheme-driven):
  `rtlib-py/rtlib_py/common/driver.py:78-156` (scheme branch, degenerate
  fallback, fetch, `_seed_input`).
- Python provider cipher construction (encryption seam):
  `rtlib-py/rtlib_py/rt_openfhe/__init__.py:364-406`, `_encrypt_values`,
  `get_input_data`, per-shard store via `_seed_input`.
- Python emit `get_encode_scheme` (real desc + placeholder):
  `ckks/src/ckks2py_driver.cxx:141-220` (desc loop, placeholder).
- C emit `Get_encode_scheme` (shared `Data_scheme_attr`):
  `poly/src/poly2c_driver.cxx:124-194` (`Emit_chunk_info`, placeholder,
  both-call `Data_scheme_attr`).
- Python sharding-emit fix (cipher-array zero-init):
  `include/fhe/ckks/ir2py_core.h:245-263`.
- C buggy `Prepare_input` (single cipher, idx 0 only):
  `rtlib/openfhe/src/openfhe_lib.cxx:43-69`.
- C ant reference `Prepare_input` (sharding):
  `rtlib/ant/ckks/src/rtlib.c:164-192`; `Pre_encode_scheme` `:67-110`.
- Fold-in commit: `d51d4dbf` (`feat(poly2py): add ckks IR->Python backend +
  rtlib-py runtime`).
- Prior C-side analysis + fix design:
  `doc/rtlib-c-bootstrap/regress58-sharding-gap-analysis.md`.
