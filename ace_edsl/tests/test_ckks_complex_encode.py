"""
Test CKKS complex plaintext encoding path in EDSL -> C codegen.
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

from ace_edsl.edsl import AceEDSL, AcePipeline, CkksCiphertext, CkksPlaintext, ckks_kernel


@ckks_kernel
def _complex_plain_mul(ct: CkksCiphertext, pt: CkksPlaintext) -> CkksCiphertext:
    return ct * pt


class TestCkksComplexEncode(unittest.TestCase):
    def test_complex_plaintext_array_emits_encode_dcmplx(self):
        AceEDSL._get_dsl.cache_clear()
        inp = CkksCiphertext(shape=(16384,), name="input")
        complex_plain = [
            1.0 + 0.25j,
            -0.5 + 0.75j,
            0.1 - 0.2j,
            0.0 + 1.0j,
        ]
        _complex_plain_mul(inp, complex_plain)

        dsl = AceEDSL._get_dsl()
        self.assertIsNotNone(dsl.current_air_module)

        pipeline = AcePipeline(dsl.current_air_module)
        pipeline.configure_fhe(
            poly_degree=16384,
            mul_level=4,
            security_level=0,
            scaling_factor_bits=56,
            first_prime_bits=60,
            hamming_weight=192,
            data_file="/tmp/test_ckks_complex_encode.msg",
            enable_poly=False,
        )
        result = pipeline.run(start_domain="fhe::ckks", dump_stages=True, verbose=False)
        self.assertTrue(result.success, msg=result.error)

        c_code = result.c_code or ""
        self.assertIn(
            "Encode_dcmplx(",
            c_code,
            "Expected complex plaintext encode path to emit Encode_dcmplx(...)",
        )


if __name__ == "__main__":
    unittest.main()

