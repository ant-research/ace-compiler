# Array Sharding for Big Model/Operation
When considering the ImageNet dataset with imagesize of [3,224,224], the input image cannot be packed into a single ciphertext for N=65536. We must partition large input/weight tensors over many "ciphertexts" in order to fit the slots within cost requirements such as rotations and memory. Tensor partitioning introduces data exchange between ciphertexts due to rotations and implict computation, and different partitioning strategies involve different costs.

We introduce the concept of sharding to represent the parallel data and computation.

## Concept
### Parallel Operator: 
runs across multiple ciphertexts and guarantees semantici equivalence.
### Sharding Specificaiton for Parallel Operator: 
How to slice input dimensions \textbf{uniformly}, arrange computation, and aggregate output.  
### Ciphertext Mesh: Derived from Sharding Strategy.
The logical device matrix, which is shared by the input and output tensor of this operator, is a one-dimensional array representing how the devices are organized.
### Tensor arrangement is derived from sharding strategy. 
It describes the distributions of the input/output tensors across ciphertext mesh. "which ciphertext gets which slice of the data". imap, omap. The tensor mapping is a two-dimensional array that represents a dimension of the tensor sliced into a dimension of the logical device matrix.

## Design
### pass ordering
1) Analysis: Analyze sharding strategy for each operator; Formal data sharding;  
t2tsharding_analysis_ctx.h  
t2tsharding_analysis_handler.h
The sharding result is saved in smap which is shared between analysis context (t2tsharding_analysis_ctx) and transformation context(t2tsharding_ctx).
2) Formal Type Lowering: New function with the new formal type (sharding). redord the mapping between original formal and new formal.
TODO: remove signature modification.
3) Transformation: 

All these handlers are wrapped in sharding_opt.cxx.

class ARRAY_SHARDING describes the distributions of the input/output tensors across computation/ciphertext mesh.
```
New_sharding_type(): Actually sharding_type is a vector.
Split_array(): Split a tensor to a vector of tensor uniformly.
New_sharding_loader(): access a sharding element.
New_sharding_store(): assign a sharding element.
New_sharding_update_store(): add a sharding element and self update.
```

## Conv Example
1) Sharding strategy analysis: how to partition each dimension of convolution.
2) Data partition: split weihgt/bias according to the sharding strategy.
3) Computation arrangement: a 2-d computation mesh. A possible arrangement:

| input_sharding[0]         | input_sharding[1]        |
|---------------------------|--------------------------|
| \*weight_sharding[0][0]   | \*weight_sharding[1][0]  |
| \*weight_sharding[0][1]   | \*weight_sharding[1][1]  |
| reduce-add                | reduce-add               |
| =                         | =                        |
| output_sharding[0]        | output_sharding[1]       |

