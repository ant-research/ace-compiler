# Vector IR

Vector IR abstracts multi-dimensional virtual vectors and their associated machine-agnostic operations. The primary objective is to enable a progressive lowering from tensor IR to vector IR and further into lower-dimensional vector representations, such as FHE and machine-native SIMD operations.

The Vector Intermediate Representation (IR) formalizes types, operations, and transformations for manipulating these vectors, enabling developers to construct domain-specific optimization passes by composing its modular components.

## IR Desing
### Types
Vector type is multi-dimensional becasuse it needs to bridge the semantic gap between tensor and low dimension operations. The dimensional information is needed in the tensor-vector lowering.
Internally array is used to represent vector type.

### A Taxonomy of Vector Operations
Features:
- Numpy compatible IR semantics (subset).
- Enough pattern abstraction to canonicalize higher-level operators.

| Taxonomy        | Operator        | Operands                                                                 | Semantic                                                                                                                                                               |
|-----------------|-----------------|--------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Computation** | add           | op0: vector<br>op1: vector                                               | Elementwise addition.<br>**Constraints:** op0 and op1 must have the same type.                                                                                        |
|                 | mul           | op0: vector<br>op1: vector                                               | Elementwise multiplication.<br>**Constraints:** op0 and op1 must have the same type.                                                                                  |
| **Shape**       | reshape       | op0: vector<br>op1: new shape (int array const)                          | Reshapes the input vector into the target shape.<br>**Return type:** Vector with shape `op1`.<br>**Example:**<br>`reshape(array_3x3, [9])` → 1D vector of length 9.    |
| **Memory**      | strided_slice | op0: array<br>op1: start indices (int array const)<br>op2: slice sizes (int array const)<br>op3: strides (int array const) | Extracts a sub-array from `op0`.<br>**Return type:** Array with shape determined by `op3`/`op2`.<br>**Example:**<br>`strided_slice(array_5x5, [1,1], [2,2], [1,1])` → 2x2 slice. |
|                 | roll          | op0: array<br>op1: shift (int const)                                     | Circularly shifts elements along an axis.<br>**Return type:** Same shape as `op0`.<br>**Example:**<br>`roll(vector_5, 3)` → Last 3 elements wrap to front.             |
|                 | tile<sup>1</sup>  | op0: vector<br>op1: repeats (int const)                                  | Repeats `op0` `op1` times.<br>**Example:**<br>`tile(vector_[1,2], 2)` → `[1, 2, 1, 2]`.                                                                                |
|                 | pad<sup>2</sup> | op0: vector<br>op1: [before, after] (int array const)                    | Expands `op0` by padding (default 0).<br>**Example:**<br>`pad(vector_[1,2], [0,1])` → `[1, 2, 0]`.                                                                     |

## IR Examples
