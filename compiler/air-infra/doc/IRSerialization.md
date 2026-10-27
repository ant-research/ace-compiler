# AIR Serialization Design and Implementation

## Overview

This document describes the design and implementation of AIR (Intermediate Representation) serialization to ELF file format. The serialization system enables saving the complete AIR state to disk and restoring it later, facilitating use cases such as:

- IR resuming between compilation phases
- IR transfer between different compilation processes
- Debugging and inspection of IR state

## Architecture

### File Format

AIR uses ELF64 format as the container for serialized data. The choice of ELF format provides:

- Standard, well-documented structure
- Support for multiple named sections
- Tool compatibility (readelf, objdump, etc.)

#### ELF Header Configuration

For IR files, the ELF header is configured as follows:

| Field | Value | Description |
|-------|-------|-------------|
| e_type | ET_NONE | Not an executable or shared object |
| e_machine | EM_NONE | No specific machine architecture |
| e_phoff | 0 | No program headers |
| e_shoff | sizeof(ELF_EHDR) | Section headers follow ELF header |
| e_shnum | 17 | Number of sections |
| e_shstrndx | 16 | Index of .shstrtab section |

#### Section Layout

| Index | Section Name | Content |
|-------|--------------|---------|
| 0 | NULL | Required empty section |
| 1 | .AIR.lit | Literal table |
| 2 | .AIR.type | Type definitions |
| 3 | .AIR.arb | Arbitrary precision types |
| 4 | .AIR.field | Field definitions |
| 5 | .AIR.param | Parameter definitions |
| 6 | .AIR.main | Main table entries |
| 7 | .AIR.aux | Auxiliary table entries |
| 8 | .AIR.cons | Constant table |
| 9 | .AIR.attr | Attribute table |
| 10 | .AIR.file | File table |
| 11 | .AIR.func_def | Function definitions |
| 12 | .AIR.blk | Block table |
| 13 | .AIR.fnhdr | Function data |
| 14 | .comment | Metadata and phase info |
| 15 | .strtab | String table |
| 16 | .shstrtab | Section header string table |

**Note**: String tables (`.strtab` and `.shstrtab`) are grouped together at the end of the section list, with `.comment` placed before them. This follows ELF conventions where `.shstrtab` is typically the last section.

### Data Structures

#### Container Serialization

The Container holds all nodes in a function's IR. The serialization format is:

```
+------------------+
| num (uint32_t)   |  Number of nodes
+------------------+
| flags_sz (uint32)|  Size of is_root bitmask
+------------------+
| is_root flags    |  Bitmask indicating root nodes
+------------------+
| offsets[num]     |  Offset of each node from data start
+------------------+
| total_sz (uint32)|  Total size of node data
+------------------+
| node data        |  Actual node data
+------------------+
```

#### Root Node Handling

Root nodes (STMTs) have a special structure:

```
STMT_DATA:
  +------------------+
  | _prev (uint32_t) |  Previous statement pointer
  +------------------+
  | _next (uint32_t) |  Next statement pointer
  +------------------+
  | NODE_DATA        |  Actual node data
  +------------------+
```

During serialization:
- `_id_array` points to NODE_DATA for all nodes
- `_sz_array` includes the STMT_DATA header for root nodes
- `is_root` flags distinguish root nodes from child nodes

## Implementation Details

### Memory Management

#### Dynamic File Expansion

The file mapping system supports dynamic expansion during writing:

```cpp
void FILE_MAP::Ensure_space(uint32_t required_size) {
  if (required_size > _map_size) {
    // Round up to next MAPPED_SIZE boundary
    uint32_t new_size = ((required_size + MAPPED_SIZE - 1) / MAPPED_SIZE) * MAPPED_SIZE;

    // Extend the file
    lseek(_fd, new_size - 1, SEEK_SET);
    write(_fd, "", 1);

    // Remap with new size
    void* new_data = mremap(_map, _map_size, new_size, MREMAP_MAYMOVE);
    _map = (char*)new_data;
    _map_size = new_size;
  }
}
```

#### Pointer Update After Remap

When the mapped region is moved by `mremap()`, all pointers derived from the old mapping become invalid. The solution is to:

1. Save the old map address before calling `Ensure_space()`
2. Call `Ensure_space()` to expand if needed
3. Update `_pos` pointer if the map address changed

```cpp
void ELF_WRITE::Ensure_space(uint32_t additional_size) {
  uint32_t required = (_pos - Get_map_addr()) + additional_size;
  BYTE_PTR old_map = Get_map_addr();
  _map->Ensure_space(required);
  if (Get_map_addr() != old_map) {
    _pos = Get_map_addr() + (_pos - old_map);
  }
}
```

### Array Resize During Recovery

When recovering Container data, the `Malloc()` call adds one entry to `_id_array`, but we need `num` entries. The `Resize()` method ensures the arrays have the correct size:

```cpp
// In ARENA_ITEM_ARRAY
void Resize(uint32_t num) {
  _id_array.resize(num, nullptr);
  _sz_array.resize(num, 0);
}

// During recovery
BYTE_PTR data = (BYTE_PTR)code->Malloc(total_sz).Addr();
code->Resize(num);  // Critical: resize before Set_item calls
```

### Size Estimation

Accurate size estimation is critical for efficient memory management. Variable-sized entries require using `Mem_size()` instead of `Size() * unit_sz`:

```cpp
size_t num = t.Size();
size_t data_size = t.Mem_size();  // Actual data size
size_t estimated_size = sizeof(uint32_t) +        // num
                        num * sizeof(uint32_t) +  // size array
                        data_size;                // actual data
```

## Bug Fixes

### Issue 1: Memory Access Beyond Mapped Region

**Symptom**: Segmentation fault when reading large IR files (>4MB)

**Root Cause**: The `Read()` function limited the mapped region to `MAPPED_SIZE` (4MB), but sections could be at offsets beyond this limit.

**Fix**: Map the entire file for reading:
```cpp
void FILE_MAP::Read(uint32_t prot, uint32_t flags) {
  // Map the entire file instead of limiting to MAPPED_SIZE
  Set_map_size(file_info.st_size);
  _map = (char*)mmap(NULL, Get_map_size(), prot, flags, Get_file_id(), 0);
}
```

### Issue 2: Array Out-of-Bounds Access

**Symptom**: Memory corruption during Container recovery

**Root Cause**: `Malloc()` adds one entry to arrays, but `Set_item()` was called for indices 0 to num-1 without resizing.

**Fix**: Added `Resize()` method and call it after `Malloc()`:
```cpp
BYTE_PTR data = (BYTE_PTR)code->Malloc(total_sz).Addr();
code->Resize(num);  // Ensure arrays have correct size
```

### Issue 3: Invalid Pointer After Remap

**Symptom**: Segmentation fault during writing large IR files

**Root Cause**: `Ensure_space()` may move the mapped region, invalidating saved offset pointers.

**Fix**: Get offset pointers AFTER calling `Ensure_space()`:
```cpp
void Archive(T& t, S s) {
  // Calculate estimated size first
  size_t estimated_size = ...;
  _elf.Ensure_space(estimated_size);

  // Get offset AFTER Ensure_space since map address may change
  BYTE_PTR offset = _elf.Get_pos();
  // ... rest of the function
}
```

### Issue 4: ELF Header Initialization

**Symptom**: Garbage values in `e_phnum` and warnings from readelf

**Root Cause**: Incomplete initialization of ELF header fields.

**Fix**: Zero the entire header before setting fields:
```cpp
void Set_ehdr() {
  memset(&_ehdr, 0, sizeof(ELF_EHDR));  // Clear all fields first
  // ... set individual fields
}
```

### Issue 5: Missing Literal Table Serialization

**Symptom**: Assertion failure `val != LITERAL_PTR()`

**Root Cause**: `Lit_table()` was not included in the serialization process.

**Fix**: Added `LIT_TAB` section and include it in `Write_glob()` and `Read_glob()`.

## Testing

### Test Cases

| Model | Size | Nodes | Phase | Result |
|-------|------|-------|-------|--------|
| add.onnx | Small | 14 | O2A | PASS |
| avg_pool.onnx | Small | 12 | O2A | PASS |
| resnet20_cifar10_pre.onnx | Medium | 373 | O2A | PASS |
| resnet110_cifar10_train.onnx | Large | 1948 | O2A | PASS |

**Note**: Currently only the O2A (ONNX to AIR) phase is tested. Other phases (FHE_SCHEME, CKKS, etc.) may require additional testing.

### Verification

The generated ELF files can be verified using standard tools:

```bash
# Check ELF header and sections
readelf -a output.B

# Verify no warnings
readelf -a output.B 2>&1 | grep -i warning

# Check file type
file output.B
```

Expected output from `file` command:
```
output.B: ELF 64-bit LSB file, no machine, version 1 (SYSV)
```

## Future Improvements

1. **Compression**: Add optional compression for large IR files
2. **Versioning**: Add format version field for forward compatibility
3. **Checksum**: Add checksum for data integrity verification
4. **Incremental Updates**: Support incremental updates without full rewrite

## References

- ELF Format Specification: https://refspecs.linuxfoundation.org/elf/elf.pdf
- Open64 Compiler .B file format (historical reference)