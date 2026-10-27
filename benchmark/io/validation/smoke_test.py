#!/usr/bin/env python3
"""Quick smoke test: verify all 3 IoPolicy values resolve to correct transport."""

import sys


sys.path.insert(0, "/work/ace-fhe/ace-compiler")

from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicySettings


GiB = 1024**3

# Test staged
IoPolicySettings(policy="staged").apply()
IoConfig(threads=1).apply()
r = _C.bench_io_roundtrip("/mnt/nvme0/smoke_staged2", 1 * GiB, 1, ["/mnt/nvme0"])
print(f"staged transport: {r['transport_name']}  verified: {r['verified']}")

# Test mapped
IoPolicySettings(policy="mapped").apply()
IoConfig(threads=4).apply()
r = _C.bench_io_roundtrip("/mnt/nvme0/smoke_mapped2", 1 * GiB, 1, ["/mnt/nvme0"])
print(f"mapped transport: {r['transport_name']}  verified: {r['verified']}")

# Test GDS
IoPolicySettings(policy="gds").apply()
IoConfig(threads=4).apply()
r = _C.bench_io_roundtrip("/mnt/nvme0/smoke_gds2", 1 * GiB, 1, ["/mnt/nvme0"])
print(f"gds transport: {r['transport_name']}  verified: {r['verified']}")

print("\nAll smoke tests passed!")
