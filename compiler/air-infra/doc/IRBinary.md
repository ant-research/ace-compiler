# IR Binary Design Document

## Overview

This document describes the design for IR binary file support in the FHE compiler. The feature enables saving and loading intermediate IR at any compilation phase, providing flexibility for debugging, testing, and incremental compilation.

## Compilation Pipeline

```
Phase 0: ONNX_PASS_MANAGER
├── O2A          (ONNX → AIR IR)
├── SCHEME_INFO  (Analyze scheme parameters)
└── VECTOR       (Vectorization)

Phase 1: FHE_COMPILER
├── SIHE         (SIHE lowering)
├── CKKS         (CKKS lowering)
├── POLY         (Polynomial lowering)
└── P2C          (Generate C code)
```

## IR Metadata Storage

IR binary files store metadata in the ELF header for easy debugging with standard tools like `readelf`.

### ELF Header Fields

| Field | Location | Description |
|-------|----------|-------------|
| Phase abbreviation | `e_ident[EI_PHASE]` (index 10-13) | 4-char phase abbreviation |
| Metadata version | `e_flags` | Version number for compatibility |

### Phase Abbreviations

| User Name | Pass Name | Abbreviation |
|-----------|-----------|--------------|
| `O2A` | `ONNX2AIR` | `O2A` |
| `SCH` | `SCHEME_INFO_ANALYZE` | `SCH` |
| `VEC` | `VECTOR` | `VEC` |
| `SIHE` | `SIHE` | `SIH` |
| `CKKS` | `CKKS` | `CKK` |
| `POLY` | `POLY` | `POL` |
| `P2C` | `POLY2C` | `P2C` |

### .comment Section

Full metadata string stored in `.comment` section for human-readable output:

```
AIR IR v1, phase: CKKS, time: 2026-04-04 20:12:34
```

View with: `readelf -p .comment <file.B>`

## Command-Line Options

### Global Options

| Option | Description |
|--------|-------------|
| `--dump=<phase>` | Dump IR after specified phase |

### Per-Pass Options

| Option | Alias | Description |
|--------|-------|-------------|
| `read_ir` | `b2ir` | Load IR from ELF file before pass runs |
| `write_ir` | `ir2b` | Save IR to ELF file after pass runs |

## Usage Scenarios

### Scenario 1: End-to-End Compilation

Generate C code directly from ONNX model.

```bash
./fhe_cmplr input.onnx -o output.c
```

### Scenario 2: Dump IR at Specific Phase

Use `--dump=<phase>` to generate IR binary at any phase. The dump file is created in the current working directory.

```bash
# Dump IR after O2A phase
./fhe_cmplr input.onnx --dump=O2A -o output.c
# Creates: input.onnx.dump.onnx2air.B

# Dump IR after CKKS phase
./fhe_cmplr input.onnx --dump=CKKS -o output.c
# Creates: input.onnx.dump.ckks.B

# Dump IR after SCH phase
./fhe_cmplr input.onnx --dump=SCH -o output.c
# Creates: input.onnx.dump.scheme_info_analyze.B
```

### Scenario 3: Resume from IR Binary

Load previously saved IR and continue compilation. The system automatically:
1. Detects `.B` file extension as IR input
2. Reads phase metadata from ELF header
3. Skips phases that have already completed
4. Continues from the appropriate phase

```bash
./fhe_cmplr ir_file.B -o output.c
```

Example output shows phase skipping:
```
Warning: ONNX2AIR PASS is skipped for IR binary (phase: O2A).
```

### Scenario 4: Per-Pass IR Dump/Load Control

Users can specify dump or load for each pass individually for fine-grained control:

```bash
# Save IR after specific pass
./fhe_cmplr input.onnx -CKKS:ir2b=ckks.B -o output.c

# Load IR before specific pass
./fhe_cmplr input.onnx -SIHE:b2ir=sihe.B -o output.c

# Dump multiple phases
./fhe_cmplr input.onnx -O2A:ir2b=o2a.B -CKKS:ir2b=ckks.B -o output.c

# Load from one phase and dump at another
./fhe_cmplr o2a.B -CKKS:ir2b=ckks.B -o output.c
```

## Implementation Details

### Key Files

| File | Purpose |
|------|---------|
| `air/util/binary/elf_info.h` | Define EI_PHASE, phase abbreviations, metadata version |
| `air/util/binary/elf_hdr.h` | Set_phase/Get_phase methods |
| `air/util/binary/elf_write.h` | Write metadata to ELF header |
| `air/driver/pass_manager.h` | Phase skipping logic, dump option handling |
| `air/driver/global_config.h` | `--dump` option definition |

### Phase Detection Logic

When loading a `.B` file:

1. Read phase abbreviation from `e_ident[EI_PHASE]`
2. Map abbreviation to phase index
3. Skip all passes with index <= saved phase index
4. Load IR at the first pass after saved phase

```cpp
static bool Should_skip_pass(const char* pass_name, const std::string& ir_phase) {
  static const char* phase_names[] = {
      "ONNX2AIR", "SCHEME_INFO_ANALYZE", "VECTOR", "SIHE", "CKKS", "POLY", "POLY2C"};
  static const char* phase_abbrs[] = {"O2A", "SCH", "VEC", "SIH", "CKK", "POL", "P2C"};

  // Find phase indices and skip if pass_idx <= ir_phase_idx
  // ...
}
```

### Dump File Naming

Dump files are generated in the current working directory (like GCC `-save-temps`):

```
<input_basename>.dump.<pass_name_lowercase>.B
```

Examples:
- `add.onnx` + `--dump=O2A` → `add.onnx.dump.onnx2air.B`
- `model.onnx` + `--dump=CKKS` → `model.onnx.dump.ckks.B`

## Verification

### Check Phase Metadata

```bash
# View phase abbreviation in ELF header
readelf -h ir_file.B
# Look for phase abbreviation at bytes 10-12 in Magic field

# View full metadata string
readelf -p .comment ir_file.B
```

Example output:
```
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 4f 32 41 00 00 00
                                        ^^^^^^^^^^^^
                                        "O2A" at position 10-12

String dump of section '.comment':
  [     0]  AIR IR v1, phase: ONNX2AIR, time: 2026-04-04 20:12:34
```

## Testing

```bash
# Test dump option
./fhe_cmplr input.onnx --dump=CKKS -o output.c
ls *.dump.ckks.B

# Test IR binary resume
./fhe_cmplr input.onnx.dump.ckks.B -o output2.c

# Verify phase skipping in trace
./fhe_cmplr input.onnx.dump.ckks.B -o output.c -t
# Check .t file for "PASS is skipped" messages
```

## References

- [IR Serialization Design](IRSerialization.md)
- Pass Manager: `air-infra/include/air/driver/pass_manager.h`
- ELF Info: `air-infra/include/air/util/binary/elf_info.h`
- Global Config: `air-infra/include/air/driver/global_config.h`