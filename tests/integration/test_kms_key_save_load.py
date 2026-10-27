#!/usr/bin/env python3
"""
End-to-end test: Phantom KMS key save and load.

  pytest tests/integration/test_kms_key_save_load.py        # run all
  pytest -v tests/integration/                               # works alongside other tests

Requires: GPU, phantom library, compiled FHE kernel

IO note
-------
On hosts where cuFile is "available" but its Bar1 mapping is non-functional (every
cuFile IO falls back to host-staged), the galois-key *load* path dead-waits on tmpfs
key directories — both the merged-file batched read (load_galois_key_merged Step2) and
the per-key parallel read_gpu. Workaround used by the staged tests in this module:
disable cuFile (ACE_GDS_ENABLED=0), single-thread the galois load
(FHERT_GALOIS_LOAD_THREADS=1), and save galois keys per-key (galois_merged_format=False).
With these, galois loads single-threaded via StagedTransport (fread+cudaMemcpy) and
completes — so the staged tests below are enabled to actually exercise the save/load
roundtrip and pin down correctness. Tests that specifically require a working cuFile
path remain skipped (see per-method markers).
"""

import os

import pytest
import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.backend import get_library_impl
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
from ace.sample.ops.specs import ADD_OP


def _provider_available(name, device):
    try:
        return get_library_impl(name, device=device).check_available()
    except Exception:
        return False


_requires_phantom = pytest.mark.skipif(not _provider_available("phantom", "cuda"), reason="phantom-cuda not available")

# Tests that exercise the cuFile/GDS path specifically — keep skipped on a
# cuFile-false host (Bar1 non-functional). The staged-I/O equivalents below cover
# the same logic without GDS.
_requires_working_cufile = pytest.mark.skip(
    reason="requires a working cuFile/Bar1 path; on this host cuFile is false-available "
    "(gds_available()=True but every cuFile IO falls back) and the galois load dead-waits. "
    "The staged-I/O variants cover the same save/load logic and are enabled."
)


class TestKMSKeySaveLoad:
    """Test Phantom key save/load for a simple add model (no bootstrap).

    Tests are split by IO path: the staged-I/O variants are enabled (they work
    around the cuFile-false-host load stall via ACE_GDS_ENABLED=0 +
    FHERT_GALOIS_LOAD_THREADS=1 + per-key galois format); the GDS-only variants
    are skipped where a working cuFile is required.
    """

    @pytest.fixture(autouse=True)
    def _staged_io_workaround(self):
        """Force host-staged, no-cuFile, per-key, single-thread galois load.

        Sidesteps the galois-key load stall on this cuFile-false host (see module
        docstring). Tests that specifically need GDS override these back as needed.
        """
        old = {k: os.environ.get(k) for k in ("ACE_GDS_ENABLED", "FHERT_GALOIS_LOAD_THREADS")}
        os.environ["ACE_GDS_ENABLED"] = "0"
        os.environ["FHERT_GALOIS_LOAD_THREADS"] = "1"
        IoPolicySettings(policy=IoPolicy.STAGED).apply()
        IoConfig(galois_merged_format=False).apply()
        try:
            yield
        finally:
            for k, v in old.items():
                if v is None:
                    os.environ.pop(k, None)
                else:
                    os.environ[k] = v

    @_requires_phantom
    @_requires_working_cufile
    def test_key_save_and_load(self, tmp_path):
        """Generate keys, save, then load with key_offline and run inference.

        Default IO (auto→GDS). Requires a working cuFile path; the staged variant
        `test_kms_with_staged_io_policy` covers the same flow without GDS.
        """
        model = ADD_OP.create_model()
        x, y = tuple(t.clone() for t in ADD_OP.example_inputs)

        # Compile and run online to generate keys
        _C.set_key_path("")
        compiled = fhe.compile(
            frontend="torch",
            library="phantom",
            device="cuda",
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)
        prog = compiled.fhe_compile((x, y))

        runner = fhe.FHERuntime(prog.package)
        result_online = runner.run(x, y)

        # Save all keys via the live runner (requires active FheContext —
        # call before finalize(). Replaces the retired OfflinePrep ctypes path.)
        key_dir = str(tmp_path / "keys")
        rc = runner.save_keys(key_dir)
        assert rc == 0, f"save_keys failed rc={rc}"
        runner.finalize()

        # Verify directory structure: global/ and model/ with .bin file
        for d in ["global", "model"]:
            dpath = os.path.join(key_dir, d)
            assert os.path.isdir(dpath), f"Missing directory {dpath}"
            bin_files = [f for f in os.listdir(dpath) if f.endswith(".bin")]
            assert len(bin_files) > 0, f"No .bin files in {dpath}"

        # Verify galois key index
        idx_path = os.path.join(key_dir, "model", "galois_key.index.json")
        assert os.path.exists(idx_path), f"Missing galois key index: {idx_path}"
        assert os.path.getsize(idx_path) > 0, f"{idx_path} is empty"

        # Load keys with key_offline mode and run inference
        package = {
            "model": ADD_OP.name,
            "kernel": prog.package["kernel"],
            "config_path": prog.package.get("config_path", ""),
            "key_path": key_dir,
            "key_mode": "key_offline",
        }
        runner2 = fhe.FHERuntime(package)
        runner2.init()
        result_offline = runner2.run(x.clone(), y.clone())
        runner2.finalize()
        _C.clear_key_path()

        # Verify inference produces valid results
        assert result_offline is not None

    @_requires_phantom
    def test_missing_key_path_fallback(self, tmp_path):
        """Explicit online mode with a stale key_path should generate keys."""
        # Set a non-existent key_path via the C API, then create FHERuntime
        # with explicit inference_mode="online" to override the auto-detection.
        _C.set_key_path("/tmp/nonexistent_kms_keys_xyz_test")

        model = ADD_OP.create_model()
        x, y = tuple(t.clone() for t in ADD_OP.example_inputs)

        compiled = fhe.compile(
            frontend="torch",
            library="phantom",
            device="cuda",
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)
        prog = compiled.fhe_compile((x, y))

        # Explicit online mode overrides the stale key_path auto-detection,
        # so keys are generated on the fly instead of loaded from disk.
        package = {
            **prog.package,
            "key_mode": "online",
        }
        runner = fhe.FHERuntime(package)
        result = runner.run(x, y)
        runner.finalize()
        _C.clear_key_path()

        assert result is not None

    @_requires_phantom
    @_requires_working_cufile
    def test_eval_only_save_and_load(self, tmp_path):
        """Save eval-only keys, verify no global/ dir, load with server_only mode.

        Default IO; requires working cuFile for the galois load.
        """
        model = ADD_OP.create_model()
        x, y = tuple(t.clone() for t in ADD_OP.example_inputs)

        # Generate keys
        _C.set_key_path("")
        compiled = fhe.compile(
            frontend="torch",
            library="phantom",
            device="cuda",
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)
        prog = compiled.fhe_compile((x, y))

        runner = fhe.FHERuntime(prog.package)
        runner.run(x, y)

        # Save eval-only keys via the live runner (before finalize).
        eval_dir = str(tmp_path / "eval_keys")
        rc = runner.save_keys(eval_dir, eval_only=True)
        assert rc == 0, f"save_keys failed rc={rc}"
        runner.finalize()

        # Verify: model/ exists, global/ does NOT
        assert os.path.isdir(os.path.join(eval_dir, "model"))
        assert not os.path.isdir(os.path.join(eval_dir, "global"))

        # Load with server_only mode — should initialize without error
        package = {
            "model": ADD_OP.name,
            "kernel": prog.package["kernel"],
            "config_path": prog.package.get("config_path", ""),
            "key_path": eval_dir,
            "key_mode": "server_only",
        }
        runner2 = fhe.FHERuntime(package)
        runner2.init()
        runner2.finalize()
        _C.clear_key_path()

    @_requires_phantom
    def test_kms_with_staged_io_policy(self, tmp_path):
        """Key save/load with staged IO policy should produce valid inference.

        Staged-I/O path (no cuFile). The autouse fixture already forces staged +
        per-key galois + single-thread load; this method additionally asserts the
        online-vs-offline numerical agreement that pins down save/load correctness.
        """

        model = ADD_OP.create_model()
        x, y = tuple(t.clone() for t in ADD_OP.example_inputs)

        # Generate keys
        _C.set_key_path("")
        compiled = fhe.compile(
            frontend="torch",
            library="phantom",
            device="cuda",
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)
        prog = compiled.fhe_compile((x, y))

        runner = fhe.FHERuntime(prog.package)
        result_online = runner.run(x, y)

        # Save all keys via the live runner (before finalize).
        key_dir = str(tmp_path / "keys_staged")
        rc = runner.save_keys(key_dir)
        assert rc == 0, f"save_keys failed rc={rc}"
        runner.finalize()

        # Load with staged IO policy
        IoPolicySettings(policy="staged").apply()
        _C.set_key_path(key_dir)

        package = {
            "model": ADD_OP.name,
            "kernel": prog.package["kernel"],
            "config_path": prog.package.get("config_path", ""),
            "key_path": key_dir,
            "key_mode": "key_offline",
        }
        runner2 = fhe.FHERuntime(package)
        runner2.init()
        result_offline = runner2.run(x.clone(), y.clone())
        runner2.finalize()

        # Reset IO policy
        IoPolicySettings(policy="auto").apply()
        _C.clear_key_path()

        # Verify results are close
        online_flat = result_online.detach().cpu().flatten().float()
        offline_flat = result_offline.detach().cpu().flatten().float()
        max_diff = (online_flat - offline_flat).abs().max().item()
        assert max_diff < 1e-3, f"Results differ (max_diff={max_diff})"

    @_requires_phantom
    def test_key_offline_predictions_match_online(self, tmp_path):
        """Key offline inference should produce numerically matching results.

        Staged-I/O path (forced by the autouse fixture). This is the core save/load
        correctness assertion: offline (loaded-key) output must match online output.
        """
        model = ADD_OP.create_model()
        x, y = tuple(t.clone() for t in ADD_OP.example_inputs)

        _C.set_key_path("")
        compiled = fhe.compile(
            frontend="torch",
            library="phantom",
            device="cuda",
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)
        prog = compiled.fhe_compile((x, y))

        runner = fhe.FHERuntime(prog.package)
        result_online = runner.run(x, y)

        key_dir = str(tmp_path / "keys_match")
        rc = runner.save_keys(key_dir)
        assert rc == 0, f"save_keys failed rc={rc}"
        runner.finalize()

        _C.set_key_path(key_dir)
        package = {
            "model": ADD_OP.name,
            "kernel": prog.package["kernel"],
            "config_path": prog.package.get("config_path", ""),
            "key_path": key_dir,
            "key_mode": "key_offline",
        }
        runner2 = fhe.FHERuntime(package)
        runner2.init()
        result_offline = runner2.run(x.clone(), y.clone())
        runner2.finalize()
        _C.clear_key_path()

        online_flat = result_online.detach().cpu().flatten().float()
        offline_flat = result_offline.detach().cpu().flatten().float()
        max_diff = (online_flat - offline_flat).abs().max().item()
        assert max_diff < 1e-3, f"Results differ (max_diff={max_diff})"

    @_requires_phantom
    @pytest.mark.parametrize("cycles", [1, 2, 3, 5])
    def test_repeated_save_load_cycles(self, tmp_path, cycles):
        """Repeat online→save→load→run N times in ONE process; each must match.

        Pinpoint test for the "FheContext rebuild residue" bug: if repeatedly
        building and destroying FheContext leaves CUDA/context residual state,
        later cycles' online or offline runs go wrong. Failing here localises the
        defect to the Prepare_context/Finalize_context cycle (NOT save/load, NOT
        keys, NOT multiprocess). Each cycle gets a fresh key dir + fresh inputs;
        we assert every cycle's offline==online within 1e-3.
        """
        model = ADD_OP.create_model()
        compiled = fhe.compile(
            frontend="torch",
            library="phantom",
            device="cuda",
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)

        diffs = []
        for i in range(cycles):
            # Fresh deterministic inputs per cycle (distinct across cycles so we
            # don't accidentally match cached results).
            torch.manual_seed(1000 + i)
            x = torch.randn(*ADD_OP.example_inputs[0].shape, dtype=torch.float32)
            y = torch.randn(*ADD_OP.example_inputs[1].shape, dtype=torch.float32)

            _C.set_key_path("")
            prog = compiled.fhe_compile((x, y))
            runner = fhe.FHERuntime(prog.package)
            result_online = runner.run(x, y)

            key_dir = str(tmp_path / f"cycle_{i}")
            rc = runner.save_keys(key_dir)
            assert rc == 0, f"cycle {i}: save_keys failed rc={rc}"
            runner.finalize()

            _C.set_key_path(key_dir)
            package = {
                "model": ADD_OP.name,
                "kernel": prog.package["kernel"],
                "config_path": prog.package.get("config_path", ""),
                "key_path": key_dir,
                "key_mode": "key_offline",
            }
            runner2 = fhe.FHERuntime(package)
            runner2.init()
            result_offline = runner2.run(x.clone(), y.clone())
            runner2.finalize()
            _C.clear_key_path()

            online_flat = result_online.detach().cpu().flatten().float()
            offline_flat = result_offline.detach().cpu().flatten().float()
            d = (online_flat - offline_flat).abs().max().item()
            diffs.append(d)

        # Every cycle must match; report which cycle first diverges.
        bad = [i for i, d in enumerate(diffs) if d >= 1e-3]
        assert not bad, (
            f"residue bug: cycles {bad} diverged (diffs={[round(d, 4) for d in diffs]}); "
            f"first bad at cycle {bad[0] if bad else -1} (0-indexed)"
        )

    @_requires_phantom
    @pytest.mark.parametrize("repeats", [1, 10])
    def test_same_context_repeat_run(self, tmp_path, repeats):
        """Run N inferences on the SAME FheContext (no rebuild) — all must match.

        Interface test to localise the residue: if the SAME context stays correct
        across many runs, then the corruption seen in `test_repeated_save_load_cycles`
        comes purely from context rebuild, not from run() itself or from save/load.
        That is the positive evidence that a non-rebuild strategy (reusing one
        context + switching keys) would avoid the phantom rebuild-residue bug.
        We check both an ONLINE context and a LOADED context.
        """
        model = ADD_OP.create_model()
        compiled = fhe.compile(
            frontend="torch",
            library="phantom",
            device="cuda",
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)

        # Fixed inputs; same context run `repeats` times must be bit-identical-ish.
        torch.manual_seed(4242)
        x0 = torch.randn(*ADD_OP.example_inputs[0].shape, dtype=torch.float32)
        y0 = torch.randn(*ADD_OP.example_inputs[1].shape, dtype=torch.float32)

        # ── ONLINE context reuse ──
        _C.set_key_path("")
        prog = compiled.fhe_compile((x0, y0))
        rt = fhe.FHERuntime(prog.package)
        outs_online = []
        for _ in range(repeats):
            outs_online.append(rt.run(x0.clone(), y0.clone()).detach().cpu().flatten().float())
        # Save keys before finalizing, for the loaded-context check below.
        key_dir = str(tmp_path / "reuse_keys")
        rc = rt.save_keys(key_dir)
        assert rc == 0, f"save_keys failed rc={rc}"
        # online self-consistency
        base = outs_online[0]
        for i, o in enumerate(outs_online):
            d = (o - base).abs().max().item()
            assert d < 1e-3, f"ONLINE context: run {i} diverged from run 0 (diff={d})"
        rt.finalize()

        # ── LOADED context reuse (preload_full, single load, many runs) ──
        _C.set_key_path(key_dir)
        package = {
            "model": ADD_OP.name,
            "kernel": prog.package["kernel"],
            "config_path": prog.package.get("config_path", ""),
            "key_path": key_dir,
            "key_mode": "key_offline",
        }
        rt2 = fhe.FHERuntime(package)
        rt2.init()
        outs_loaded = []
        for _ in range(repeats):
            outs_loaded.append(rt2.run(x0.clone(), y0.clone()).detach().cpu().flatten().float())
        # loaded self-consistency AND loaded vs online agreement
        lbase = outs_loaded[0]
        for i, o in enumerate(outs_loaded):
            d = (o - lbase).abs().max().item()
            assert d < 1e-3, f"LOADED context: run {i} diverged from run 0 (diff={d})"
        # the first loaded run must agree with the first online run (key roundtrip ok)
        d_ol = (lbase - base).abs().max().item()
        assert d_ol < 1e-3, f"loaded vs online differ (diff={d_ol})"
        rt2.finalize()
        _C.clear_key_path()
