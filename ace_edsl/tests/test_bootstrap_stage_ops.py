"""
Test first-class CKKS bootstrap stage ops and runtime lowering.
"""

import os
import subprocess
import sys
import textwrap
import unittest


class TestBootstrapStageOps(unittest.TestCase):
    def test_stage_ops_lower_to_context_runtime_calls(self):
        repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
        parent_root = os.path.abspath(os.path.join(repo_root, ".."))
        script = textwrap.dedent(
            f"""
            import os
            import sys
            sys.path.insert(0, {repo_root!r})
            sys.path.insert(0, {parent_root!r})
            from ace_edsl.edsl import AceEDSL, AcePipeline, CkksCiphertext, ckks_kernel
            os.environ["ACE_BOOTSTRAP_STAGE_PRIMITIVE_LOWERING"] = "0"

            @ckks_kernel
            def _bootstrap_stage_kernel(ct: CkksCiphertext) -> CkksCiphertext:
                x = ct.raise_mod(2)
                x = x.bootstrap_coeffs_to_slots()
                x = x.bootstrap_eval_mod()
                x = x.bootstrap_slots_to_coeffs()
                return x

            AceEDSL._get_dsl.cache_clear()
            inp = CkksCiphertext(shape=(16384,), name="input")
            _bootstrap_stage_kernel(inp)
            dsl = AceEDSL._get_dsl()
            before_ir = dsl.current_air_module.dump().lower()
            assert "ckks.bootstrap_coeffs_to_slots" in before_ir
            assert "ckks.bootstrap_eval_mod" in before_ir
            assert "ckks.bootstrap_slots_to_coeffs" in before_ir

            pipeline = AcePipeline(dsl.current_air_module)
            pipeline.configure_fhe(
                poly_degree=16384,
                mul_level=8,
                security_level=0,
                scaling_factor_bits=56,
                first_prime_bits=60,
                hamming_weight=192,
                data_file="/tmp/test_bootstrap_stage_ops.msg",
                enable_poly=True,
            )
            result = pipeline.run(start_domain="fhe::ckks", dump_stages=True, verbose=False)
            assert result.success, result.error
            c_code = result.c_code or ""
            assert "Eval_bootstrap_coeffs_to_slots_ciph(" in c_code
            assert "Eval_bootstrap_eval_mod_ciph(" in c_code
            assert "Eval_bootstrap_slots_to_coeffs_ciph(" in c_code
            print("STAGE_OP_TEST_OK")
            """
        )
        proc = subprocess.run(
            [sys.executable, "-c", script],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
        )
        self.assertEqual(proc.returncode, 0, msg=f"{proc.stdout}\n{proc.stderr}")
        self.assertIn("STAGE_OP_TEST_OK", proc.stdout)

    def test_stage_ops_can_lower_to_explicit_primitives(self):
        repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
        parent_root = os.path.abspath(os.path.join(repo_root, ".."))
        script = textwrap.dedent(
            f"""
            import os
            import sys
            sys.path.insert(0, {repo_root!r})
            sys.path.insert(0, {parent_root!r})
            from ace_edsl.edsl import AceEDSL, AcePipeline, CkksCiphertext, ckks_kernel

            os.environ["ACE_BOOTSTRAP_STAGE_PRIMITIVE_LOWERING"] = "1"

            @ckks_kernel
            def _bootstrap_stage_kernel(ct: CkksCiphertext) -> CkksCiphertext:
                x = ct.bootstrap_coeffs_to_slots()
                x = x.bootstrap_eval_mod()
                x = x.bootstrap_slots_to_coeffs()
                return x

            AceEDSL._get_dsl.cache_clear()
            inp = CkksCiphertext(shape=(16384,), name="input")
            _bootstrap_stage_kernel(inp)
            dsl = AceEDSL._get_dsl()
            before_ir = dsl.current_air_module.dump().lower()
            assert "ckks.bootstrap_coeffs_to_slots" not in before_ir
            assert "ckks.bootstrap_eval_mod" not in before_ir
            assert "ckks.bootstrap_slots_to_coeffs" not in before_ir
            assert "ckks.rotate" in before_ir
            assert "ckks.mul" in before_ir

            pipeline = AcePipeline(dsl.current_air_module)
            pipeline.configure_fhe(
                poly_degree=16384,
                mul_level=8,
                security_level=0,
                scaling_factor_bits=56,
                first_prime_bits=60,
                hamming_weight=192,
                data_file="/tmp/test_bootstrap_stage_ops_primitive.msg",
                enable_poly=True,
            )
            result = pipeline.run(start_domain="fhe::ckks", dump_stages=True, verbose=False)
            assert result.success, result.error
            c_code = result.c_code or ""
            assert "Eval_bootstrap_coeffs_to_slots_ciph(" not in c_code
            assert "Eval_bootstrap_eval_mod_ciph(" not in c_code
            assert "Eval_bootstrap_slots_to_coeffs_ciph(" not in c_code
            assert ("Rotate_ciph(" in c_code or "Rotate(" in c_code)
            assert ("Mul_ciph(" in c_code or "Hw_modmul" in c_code)
            print("STAGE_OP_PRIMITIVE_LOWERING_OK")
            """
        )
        proc = subprocess.run(
            [sys.executable, "-c", script],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
        )
        self.assertEqual(proc.returncode, 0, msg=f"{proc.stdout}\n{proc.stderr}")
        self.assertIn("STAGE_OP_PRIMITIVE_LOWERING_OK", proc.stdout)


if __name__ == "__main__":
    unittest.main()
