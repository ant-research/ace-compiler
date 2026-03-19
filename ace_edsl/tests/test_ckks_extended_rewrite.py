"""
Test CKKS extended-op primitive rewrite pass.
"""

import os
import sys
import unittest


def _setup_sys_path():
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    parent_root = os.path.abspath(os.path.join(repo_root, ".."))
    for path in (repo_root, parent_root):
        if path not in sys.path:
            sys.path.insert(0, path)


_setup_sys_path()

from ace_edsl.edsl import AceEDSL, AcePipeline, CkksCiphertext, ckks_kernel


@ckks_kernel
def _extended_ops_kernel(ct: CkksCiphertext) -> CkksCiphertext:
    x = ct.raise_mod(2)
    y = x.conjugate()
    z = y.mul_mono(1)
    return z


class TestCkksExtendedRewrite(unittest.TestCase):
    def test_rewrite_removes_extended_ops(self):
        AceEDSL._get_dsl.cache_clear()
        inp = CkksCiphertext(shape=(16384,), name="input")
        _extended_ops_kernel(inp)

        dsl = AceEDSL._get_dsl()
        self.assertIsNotNone(dsl.current_air_module)
        before_ir = dsl.current_air_module.dump().lower()
        self.assertIn("ckks.raise_mod", before_ir)
        self.assertIn("ckks.conjugate", before_ir)
        self.assertIn("ckks.mul_mono", before_ir)

        pipeline = AcePipeline(dsl.current_air_module)
        pipeline.configure_fhe(
            poly_degree=16384,
            mul_level=8,
            security_level=0,
            scaling_factor_bits=56,
            first_prime_bits=60,
            hamming_weight=192,
            data_file="/tmp/test_ckks_extended_rewrite.msg",
            enable_poly=True,
        )
        pipeline.set_ckks_extended_op_rewrite(True)
        result = pipeline.run(start_domain="fhe::ckks", dump_stages=True, verbose=False)
        self.assertTrue(result.success, msg=result.error)

        rewritten_ir = result.air_dumps.get("ckks_extended_rewrite", "").lower()
        self.assertNotIn("ckks.raise_mod", rewritten_ir)
        self.assertNotIn("ckks.conjugate", rewritten_ir)
        self.assertNotIn("ckks.mul_mono", rewritten_ir)

        c_code = result.c_code or ""
        self.assertNotIn("Raise_mod(", c_code)
        self.assertNotIn("Conjugate_ciph(", c_code)
        self.assertNotIn("Mul_mono_ciph(", c_code)
        self.assertTrue(
            ("Add_ciph(" in c_code) or ("Hw_modadd" in c_code),
            "Expected rewritten primitive path to include add primitive",
        )


if __name__ == "__main__":
    unittest.main()
