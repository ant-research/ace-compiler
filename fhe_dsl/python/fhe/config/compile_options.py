# config/compile_options.py
from dataclasses import dataclass, field
from typing import Optional, List, Union, Dict, Any
from .base import BaseOption

@dataclass
class FHEConfig:
  scheme: str = "CKKS"
  poly_modulus_degree: int = 8192
  multiplication_depth: int = 2
  backend: str = "CPU"
  # ... Other parameters

@dataclass
class CompileOptions(BaseOption):
    """
    Compilation-time configuration options.

    These options control how the function is compiled to FHE.

    Compiler options (vec, ckks, sihe, p2c) are passed as dicts and converted
    to command-line arguments for fhe_cmplr.

    Example:
        CompileOptions(
            encrypt_inputs=["x"],
            vec={"ms": 256},
            ckks={"N": 4096, "hw": 192}
        )
    """
    encrypt_inputs: Union[List[int], List[str], None] = None
    config: FHEConfig = field(default_factory=FHEConfig)

    # Compiler options passed to fhe_cmplr
    # vec: Vectorization options (e.g., {"ms": 256} for max_slots)
    vec: Optional[Dict[str, Any]] = None
    # ckks: CKKS scheme options (e.g., {"N": 4096, "hw": 192})
    ckks: Optional[Dict[str, Any]] = None
    # sihe: SIHE options (e.g., {"relu_vr_def": 2})
    sihe: Optional[Dict[str, Any]] = None
    # p2c: Poly2C options (e.g., {"fp": True})
    p2c: Optional[Dict[str, Any]] = None
    # o2a: O2A options (e.g., {"ts": True})
    o2a: Optional[Dict[str, Any]] = None
    # fhe_scheme: FHE scheme options (e.g., {"ts": True})
    fhe_scheme: Optional[Dict[str, Any]] = None
    # poly: Polynomial options (e.g., {"ts": True, "rtt": True})
    poly: Optional[Dict[str, Any]] = None

    # ReLU Value Range data for AIR IR embedding (Python-side, not CLI)
    # relu_vr_data: explicit dict mapping AIR node name to VR float value
    #   e.g. {"relu_Relu": 4.0, "layer1_0_relu_Relu": 5.0}
    # relu_vr_file: path to JSON profile file (loaded via load_vr_file)
    # profile_relu: if True, profile ReLU VR from example_inputs during compile
    relu_vr_data: Optional[Dict[str, float]] = None
    relu_vr_file: Optional[str] = None
    profile_relu: bool = False

def _dict_to_cmd_args(config: dict) -> list:
    """Convert structured config dict to command line arguments.

    Supports multiple formats:
    - Simple flags: {"vec": {"conv_parl": True}} → "-VEC:conv_parl"
    - Key-value: {"ckks": {"N": 65536}} → "-CKKS:N=65536"
    - Multiple values: {"ckks": {"q0": 60, "sf": 56}} → "-CKKS:q0=60:sf=56"
    - String values (passed as-is): {"sihe": {"relu_vr": "/relu/Relu=4;..."}} → "-SIHE:relu_vr=/relu/Relu=4;..."
    """
    cmd_args = []

    for prefix, options in config.items():
        if not options:
            continue

        prefix_upper = prefix.upper()

        if prefix == "vec":
            args = ["-VEC"]
            for key, value in options.items():
                if isinstance(value, bool) and value:
                    args.append(key)  # Flag without value
                else:
                    args.append(f"{key}={value}")
            cmd_args.append(":".join(args))

        elif prefix == "ckks":
            args = ["-CKKS"]
            for key, value in options.items():
                if isinstance(value, bool) and value:
                    args.append(key)
                elif key == "relu_vr" and isinstance(value, dict):
                    # Special handling for relu_vr mapping
                    mapping = ";".join([f"{k}={v}" for k, v in value.items()])
                    args.append(f"relu_vr={mapping}")
                elif isinstance(value, str) and (";" in value or "=" in value):
                    # String with semicolons or equals - pass as-is without extra quotes
                    args.append(f"{key}={value}")
                else:
                    args.append(f"{key}={value}")
            cmd_args.append(":".join(args))

        elif prefix == "p2c":
            # P2C may have multiple independent parameters
            for key, value in options.items():
                if isinstance(value, bool) and value:
                    cmd_args.append(f"-P2C:{key}")
                else:
                    cmd_args.append(f"-P2C:{key}={value}")

        elif prefix == "sihe":
            args = ["-SIHE"]
            for key, value in options.items():
                if isinstance(value, bool) and value:
                    args.append(key)
                elif key == "relu_vr" and isinstance(value, dict):
                    mapping = ";".join([f"{k}={v}" for k, v in value.items()])
                    args.append(f"relu_vr={mapping}")
                elif isinstance(value, str) and (";" in value or "=" in value):
                    # String with semicolons or equals - pass as-is without extra quotes
                    args.append(f"{key}={value}")
                else:
                    args.append(f"{key}={value}")
            cmd_args.append(":".join(args))

        # Other prefixes...
        elif prefix in ["o2a", "fhe_scheme", "poly"]:
            base = f"-{prefix_upper}"
            args = [base]
            for key, value in options.items():
                if isinstance(value, bool) and value:
                    args.append(key)
                else:
                    args.append(f"{key}={value}")
            cmd_args.append(":".join(args))

    return cmd_args
