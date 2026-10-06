# rtlib-py documentation set

This set documents the rtlib-py Python compilation path: `fhe_cmplr -CKKS2PY` lowers an ONNX model through an `ir2py` pass to a `.py` that runs on a pure-Python runtime whose providers all share a frozen 39-name contract. Three providers ship out of the box -- a plaintext reference and two real-CKKS backends -- so the same emitted program can run for fast validation or for actual FHE execution. rtlib-py is a separate layer from the C-layer `rtlib`.

## Flag-policy end state

The `-CKKS2PY:` flag group has one activator: `-CKKS2PY:rt=<module>` is the sole activator of the Python emit pass and selects the Python provider module (`rt_ref`, `rt_openfhe`, `rt_fhert`, or `rt_<name>`). The `-CKKS2PY:lib=` sub-flag is removed -- it is rejected at parse time, not a soft-ignored option; activation, the Python module selection, and the derived `_provider` tag flow strictly through `rt=`. A `-CKKS2PY:fp` bare flag inserts `free_ciph` calls (result-neutral on every provider). The end-state design note is [`doc/ckks2py-flag-policy.md`](../ckks2py-flag-policy.md).

## Table of contents

| Doc | Contents | Audience |
|---|---|---|
| [`10-new-backend-integration.md`](10-new-backend-integration.md) | **User guide: compiling, running, `df`, and adding providers** -- how to compile ONNX to the rtlib-py Python path, run it on a provider, externalize static data with `-CKKS2PY:df`, choose a provider, and add a new runtime provider (onboarding) | Users running/integrating the compiler; backend integrators |
| [`20-implementation-status.md`](20-implementation-status.md) | **Implementation: pipeline and provider internals** -- the `ir2py` emission pass, the rtlib-py common layer (contract/driver/registry/timing), the three providers and their design decisions, the `df` externalization mechanism, emitter integration, build prerequisites, code size | Engineers maintaining/extending the compiler |
| [`30-test-status.md`](30-test-status.md) | **Validation: test suites and results** -- what is validated (regress 58, the resnet batch, non-regress appendix, archived rt_openfhe comparison, post-relocation regress58 + resnet20 (n=1) re-verification across all three providers), configs/backends, what passed/failed/untested, result terminology, the `df` on/off gate, reproduction data | Reviewers / testers |
| [`40-open-issues.md`](40-open-issues.md) | **Limitations and open issues** -- canonical single source of truth for generalized compiler limitations: unsupported/unimplemented features, known defects, upstream/config restrictions, `df`-related limits, performance/scalability, driver/tooling coverage; each with scope, impact, workaround, and evidence link | All readers assessing fitness |
| [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md) | **Case study: real-world resnet benchmark** -- deep-dive on resnet inference on real CKKS (`rt_openfhe` + bootstrap): the resnet20 verification, root-cause analysis of bootstrap depth/scaling issues, the env-gated fixes, and evaluation scope | Readers evaluating real-CKKS accuracy (optional deep-dive) |

## Recommended reading path

- **Users** wanting to compile, run, or add a backend -> [`10-new-backend-integration`](10-new-backend-integration.md).
- **Internals/maintenance** (pipeline, providers, `df` mechanism) -> [`20-mplementation-status`](20-implementation-status.md).
- **Validation** (which suites exist and what passed) -> [`30-test-status`](30-test-status.md).
- **Fit/limitations** (unsupported features, known defects) -> [`40-open-issues`](40-open-issues.md).
- **Real-CKKS accuracy** (optional resnet case study) -> [`50-resnet-benchmark-case-study`](50-resnet-benchmark-case-study.md).
