# -CKKS2PY flag-policy end state

The `-CKKS2PY` compiler pass emits Python from CKKS IR
(`from rtlib_py.<module> import *`) instead of C. It is inactive by
default: with no `-CKKS2PY` on the command line, the compiler emits C
output byte-identical to a build without the pass. This note fixes the
END-STATE flag policy: the user-facing `-CKKS2PY:` sub-flags, the
activation rule, the removed sub-flag, the free-cipher gate, and the
on-disk home of the headers. The headers and pass driver in this
tree are already at the end state, EXCEPT for a small set of stale
comments F5.2 will clean; no live code changes will be needed.

## The `rt=` sole activator

The `-CKKS2PY:rt=<module>` sub-flag is the SOLE activator of the
Python emit. It sets `CKKS2PY_CONFIG::_py_rt_module` (a `std::string`
member) via the air-util option parser; see the
`OPTION_DESC Ckks2py_option[]` table in `ckks/src/ckks2py_config.cxx`
(`rt`, `rt_module`, V_EQUAL, K_STR). A non-empty value activates the
pass; an empty value keeps it inactive.

- `CKKS2PY_CONFIG::Py_rt_empty()` (in
  `include/fhe/ckks/ckks2py_config.h`) returns `_py_rt_module.empty()`.
- `CKKS2PY_PASS::Pre_run()` (in `ckks/src/ckks2py_pass.cxx`) sets
  `_active = false` and returns when `Py_rt_empty()` is true;
  otherwise `_active = true` and disables the CKKS->poly IR pass and
  the poly2c C emitter, so the Python driver is the only emitter run.
- `CKKS2PY_PASS::Run()` is a no-op when `!_active`.

With no `-CKKS2PY:rt=`, the default C pipeline runs and the emitted C
is byte-identical to a build that does not carry the pass. The
accepted `<module>` values are the rtlib-py submodule names
`rt_ref`, `rt_openfhe`, `rt_fhert`; the emitted module top-level
imports one of them via `from rtlib_py.<module> import *` (see
`IR2PY_CTX::Emit_global_include` in `include/fhe/ckks/ir2py_ctx.h`),
and a runner fetches the provider implementation from that module.

## The `lib=` removal

The `-CKKS2PY:lib=` sub-flag is removed from the end state. It is NOT
in the registered `Ckks2py_option[]` table in `ckks2py_config.cxx`, so
the air-util option parser rejects any `-CKKS2PY:lib=*` token as an
unknown group member. Activation flows strictly through `rt=`; the
pass never reads `lib=`. The `_provider` field is derived from `rt=`
alone in `CKKS2PY_CONFIG::Update_options()`: empty `rt=` ->
`PROVIDER::INVALID` (pass inactive); `rt_openfhe` / `rt_fhert` ->
`PROVIDER::OPENFHE`; `rt_ref` and other module names ->
`PROVIDER::INVALID`, which `IR2PY_CTX::Provider_tag()` turns into the
honest module-name tag.

A small set of stale `lib=` and "step 2" comments remain in the
headers and passes; F5.2 will refresh them. The live option table and
`Update_options()` are already at the end state, so F5.2 makes no
code changes. Sites F5.2 will touch:
`include/fhe/ckks/ckks2py_pass.h:43-44`,
`include/fhe/ckks/ckks2py_config.h:40-41,53,60`,
`include/fhe/ckks/ir2py_ctx.h:66,230`,
`ckks/src/ckks2py_pass.cxx:35-38`, `ckks/src/ckks2py_config.cxx:49-54`.

## `fp` gates `mfree` (free_ciph)

The `-CKKS2PY:fp` sub-flag is a no-value flag (`V_NONE` + `K_NONE`).
Setting it flips `_free_poly` to true in the config (inherited from
`POLY2C_CONFIG`). The driver's per-function body emission
(`CKKS2PY_DRIVER::Run` in `include/fhe/ckks/ckks2py_driver.h`) then
runs `fhe::ckks::MFREE_PASS` on the body, inserting `OPC_FREE` nodes
for dead ciphers/plains right before emission;
`IR2PY_HANDLER::Handle_free` (in `include/fhe/ckks/ir2py_handler.h`)
lowers each `OPC_FREE` to a `free_ciph(a)` / `free_plain(a)` call.
Without `-CKKS2PY:fp`, no `free_ciph` / `free_plain` calls are
emitted; the Python runtime's GC owns the handles and the output
values do not change. `fp` is an opt-in parity/cosmetic knob, off by
default, not a resource-correctness requirement on any reachable
Python provider (`rt_ref`, `rt_openfhe`, `rt_fhert`).

## The `_prov_str` plumbing never lands in v2

`POLY2C_CONFIG` (in `include/fhe/poly/poly2c_config.h`) has a
`std::string _prov_str` member used by the C POLY2C path. The Python
backend never adds ANY new `_prov_str` plumbing to its own config,
driver, ctx, or handler: no new `_prov_str` member in
`CKKS2PY_CONFIG`, no `Prov_str()` accessor on the Python path, and no
provider string plumbed through the Python ctx/handler from a `lib=`
source. The provider tag at emit time is synthesized from `rt=` via
`IR2PY_CTX::Provider_tag()` (in `include/fhe/ckks/ir2py_ctx.h`):
`PROVIDER::OPENFHE` -> `"LIB_OPENFHE"`; otherwise the rt module name.
The module name (from `rt=rt_openfhe` / `rt_fhert` / `rt_ref`) is the
SOLE source for the provider tag AND the SOLE source for the
rtlib-py import; `lib=` and `_prov_str` do not appear on the Python
surface. A small set of comments in
`include/fhe/ckks/ckks2py_config.h` reference `_prov_str` while
explaining why the inherited base member is NOT used by the Python
path; F5.2 refreshes those references to the end-state wording.

## poly vs ckks: location rationale

The Python backend headers sit at `include/fhe/ckks/`
(`ckks2py_pass.h`, `ckks2py_config.h`, `ckks2py_driver.h`,
`ir2py_ctx.h`, `ir2py_handler.h`, `ir2py_core.h`, `ckks2c_mfree.h`)
in the `fhe::ckks` namespace; this is their durable home, not a
relocation step. The headers landed at this path ab initio in this
branch, so there is no poly->ckks move to describe. `include/fhe/poly/`
remains the home of the C POLY2C backend (`poly2c_pass.h`,
`poly2c_config.h`, `poly2c_driver.h`, `poly2c_mfree.h`);
`CKKS2PY_CONFIG` inherits from `fhe::poly::POLY2C_CONFIG` so the
existing IR2C_CTX chain (which stores `const POLY2C_CONFIG& _config`,
see `fhe/core/ir2c_ctx.h`) binds to a `CKKS2PY_CONFIG` and reads the
inherited provider/data_file/config-file accessors unchanged. The
inheritance is a deliberate reuse of base-class accessors, NOT a
relocation of the Python pass. No alternate spelling of the
`-CKKS2PY:` option group appears in the committed tree (the group is
named `CKKS2PY` only).

## Summary

- `rt=<rt_ref|rt_openfhe|rt_fhert>` is the sole activator
  (`_py_rt_module`); empty = inactive, default C output byte-identical.
- `lib=` is removed (not a registered option; the parser rejects any
  `-CKKS2PY:lib=*` token). Stale `lib=` / "step 2" comments F5.2 will
  clean; no code change.
- `fp` gates `mfree` (free_ciph / free_plain); off by default.
- No new `_prov_str` plumbing on the Python path; the provider tag is
  derived from `rt=` via `IR2PY_CTX::Provider_tag()`.
- Headers sit at `include/fhe/ckks/` (the `fhe::ckks` namespace home),
  ab initio; `include/fhe/poly/` stays the C POLY2C home,
  inherited-not-relocated by the Python config.
