# Array Sharding for Large Operation/Model
When considering the ImageNet dataset with imagesize of [3,224,224], the input image cannot be packed into a single ciphertext for N=64k. We must partition large input/weight tensors over many "ciphertexts" in order to fit the slots within cost requirements such as rotations and memory. Tensor partitioning introduces data exchange between ciphertexts due to rotations and implict computation, and different partitioning strategies involve different costs.

We introduce the concept of sharding to represent the parallel data and computation.

## Domain Problems (CKKS)
### 1. Computation Mesh

### 2. Replication driven conv for performance: balancing utilization and rotation
For sharding with _utilization-first partitioning strategy_, the data size of each shard is as close to the number of slots as possible. In some scenarios, this may lead to a rotation flush issue, where the rotation of one ciphertext flushes itself.
- Cyclic roll is need by two CKKS rotations and one mul level.
- Metakernel block formation is limited for 1x1 convolution.

Example: N=2k, S=1k  
**conv2d**: input[64, 7, 7], kernel[16, 64, 1, 1], output[16, 7, 7]  
- Mesh[x=1, y=4, z=1]: input is replicated in 4 mesh devices.
each block: input[i*16:(i+1)*16, 7, 7], kernel[16, i*16:(i+1)*16, 1, 1], output[16, 7, 7]@mesh[y=i]
rotations: 16*2=32
Total: 4*16*2=128
- Mesh[x=2, y=8, z=1]: 512 slots
each block: input[j*8:(j+1)*8, 7, 7], kernel[8, i*8:(i+1)*8, 1, 1], output[8, 7, 7]@mesh[x=i, y=j]
rotations: 8+1
Total: 16*9=144

Conv in ResNet50 (N=64K): input[2048,7,7], kernel[512, 2048, 1, 1], output[512,7,7]
- Mesh[x=1, y=4, z=1]: 32K slots. Rotations: 4*(512*2)=4096.
- Mesh[x=2, y=8, z=1]: 16K slots. Rotations: 16*(16+32)=768.



### 3. Representation of non-shard for an operation
Actually non-shard input is replication, and each shard load the input. So solve it in loader.
Example: N=2048, S=1024  
**conv2d**: input[16, 7, 7], kernel[64, 16, 1, 1], output[64, 7, 7]  
Mesh[x=4, y=1, z=1]: input is replicated in 4 mesh devices.
each block: input[16, 7, 7], kernel[i*16:(i+1)*16, 16, 1, 1], output[i*16:(i+1)*16, 7, 7]@mesh[x=i]


## Concepts and Datastructure
Sharding is a mechanism to distribute computation across multiple computation devices/ciphertexts by splitting an array into smaller blocks called shards.
Sharding = Partitioning + Scheduling @ Computation Mesh.

### 1. Computation Mesh
A computation mesh is an N-dimensional array with named axes that provides a logical view of the available devices. The mesh structure typically mirrors the underlying system's topology. In the context of FHE, the devices are represented by ciphertexts.

Why 3d in implementation? Actually it can be ND according the underlying device organization. 

typedef struct {
  unsigned int x; 
  unsigned int y; 
  unsigned int z; 
} DIM3;

Example:
| mesh             | devices               |
| ---------------- | --------------------- |
| [x=3, y=1, z=2]  | Total 6 ciphertexts   |
| [x=1, y=64, z=2] | Total 128 ciphertexts |


composable parallelism 


DIM3 conv_mesh [x=1, y=2, z=2] means the convolution computation will be distributed in 2x2 devices for the computation.
 m00 m01
 m10 m11
Each mesh device will get the mapped data blocks to finish a small conv.

conv[3,224,224]
input [3, 224, 224] by map(C) @ mesh[x=1, y=2, z=2]
 

### 2. ArraySharding 
DataSharding describes the distribution of array across device mesh.

```
// DataSharding = mesh + partition
// which can derive the array distribution in each mesh device.
class ARRAY_SHARDING {
  DIM3 mesh;
  // Each element describes how an array dimension is partitioned across mesh axis.
  std::vector<int> spec;

  Print();
}
```
DArray (Array by DataSharding) is represented by 2D array: [n][block_size].  

**<u>Example 1</u>**:
| Array[C, H, W] | ArraySharding                           | DArray        |
| -------------- | --------------------------------------- | ------------- |
| A[3, 224, 224] | mesh={x=4, y=3, z=2}<br>spec={y, z, -1} | D[6][112x224] |

Distribution on ciphertext mesh (implicit replicate in mesh-x axis):

| mesh[x,0,0] | mesh[x,0,1] | mesh[x,1,0] | mesh[x,1,1] | mesh[x,2,0] | mesh[x,2,1] |
| ----------- | ----------- | ----------- | ----------- | ----------- | ----------- |
| D[0]        | D[1]        | D[2]        | D[3]        | D[4]        | D[5]        |


**<u>Example 2</u>**:
| Array[C, H, W] | ArraySharding                           | DArray        |
| -------------- | --------------------------------------- | ------------- |
| A[64, 56, 56]  | mesh={x=8, y=8, z=1}<br>spec={y, z, -1} | D[8][8x56x56] |

Distribution on ciphertext mesh (implicit replicate in x axis):

| mesh[x,0,0] | mesh[x,1,0] | mesh[x,2,0] | mesh[x,3,0] | ... | mesh[x,7,0] |
| ----------- | ----------- | ----------- | ----------- | --- | ----------- |
| D[0]        | D[1]        | D[2]        | D[3]        | ... | D[7]        |


### 3. Indexer, loader and storer
Index by mesh.

Sharding Indexer for conv:
  // y(C) input/weight
  // ^
  // | z(H) input/output
  // |/
  //  -----> x (F) output/weight/bias

input: yiv*z + ziv
weight: xiv*y + yiv 
output: xiv*z + ziv


for yiv to y: // input shard in C dim
  for ziv to z: // H dim
    // kernel_size * kernel_size rotations: 7*7=49
    input2d = transform_rot2d(input[yiv*z + ziv])
    for xiv to x: // output shard in F dim 
        st output[xiv*z + ziv]
          conv_vector(input2d, weight_d)


Original:
for xiv to x: // output shard in F dim 
  for yiv to y: // input shard in C dim 
    for ziv to z: // H dim 
        input2d = transform_rot2d(input[yiv*z + ziv])
        st output[xiv*z + ziv]
          conv_vector(input2d, weight_d)

Time breakdown for conv: input [3][224][224], weight[64][3][7][7], stride=2
Sharding for N=65K, Slots=32K, mul_depth=3

for yiv to 3:    // C=3
  for ziv to 2:  // H dim
    input2d = transform_rot2d(input[yiv*z + ziv], 7*7)
    for xiv to 64: // F=64 dim
      st output[xiv*z + ziv]
        stride_slice -> [56, 112]
          conv_vector(input2d, weight_d) // rtype=[112, 224]
      
for xiv:
  conv_vector:   706 ms  // level-3
  slice-112:   8,540 ms  // level-2
  slice-56:    3,390 ms  // level-1
  Total:      12,636 ms

3*2*64* 12.636s = 4838 s



### ComputationSharding
Parallel Operator runs across multiple devices and guarantees semantici equivalence.
```
// ComputationSharding: mesh and input/output partition spec
class OP_SHARDING {
  DIM2 mesh;
  // How input is sharded
  stc::vector<ARRAY_SHARDING> imap;
  // How output is sharded
  stc::vector<ARRAY_SHARDING> omap;

  ARRAY_SHARDING Get_imap(int i);
  ARRAY_SHARDING Get_omap(int i);
  DIM2 Get_mesh();
  Print();
}
```

### ShardingMap maps 
slices up inputs into blocks (and the output is formed by concatenating result blocks), 
the mesh argument lets us control precise device placement of computation and results;

```
//Global Sharding Map for the model:
class SHARDING_MAP {
  std::map<NODE_PTR, OP_SHARDING*> op_sharding_map;
  
  bool Is_sharding_op(NODE_PTR op); // 
  Set_op_sharding(NODE_PTR op, OP_SHARDING*);
  Get_op_sharding(NODE_PTR op, OP_SHARDING*);
}
```



## Design: sharding as program transforms
Actually you can think sharding strategy as "scheduler" which orangizes computation in a computation mesh.

### pass ordering
Analysis->Propagation->Opts->Lowering(to Single Core Multiple Ciphertexts)
1) Analysis: Analyze sharding strategy for each operator.
Start from "input" or user annotated sharding to drive the initial sharding strategy.
computation follows sharding. Analysis only compute info in the ShardingMap.
t2tsharding_analysis_ctx.h  
t2tsharding_analysis_handler.h
The sharding result is saved in smap which is shared between analysis context (t2tsharding_analysis_ctx) and transformation context(t2tsharding_ctx).

2) Sharding Propagation: propagate and infer sharding infomation for all ops in the model. 
TODO:forward+backward with cost
When the output tensor model of an operator is inconsistent with the input tensor model of the next operator, computation and communication operations need to be introduced to implement the change between tensor layouts. The automatic parallel process introduces the tensor redistribution algorithm, which can be used to derive the communication conversion operations between random tensor layouts. 

Drive the formation of EltwiseShard and ReduceShard.
Reshard is moved to this pass.


Why reshard: 
Perf/mem driven: less devices/ciphertexts. mesh or data distribution changes.
consistency 
    between ops: between meshes.
    consistency inside op: inputs, input-output.

reshard(data_shard_in, mesh) -> data_shard_out: 

Who drives reshard?
    Propagation

What info is needed by propagation?
    op sharding rule


Limitations:
复杂的Reshard：
1）因为和Lowering过程交错。决策发生在shape变化的op上（conv2d，gemm），依据时shape变化，同时改变tensor的Ciphertext分布。
input->conv1(stride=2)->output1->conv2()->output2  
conv1是决策点，比如stride， channel_out。但reshard的分布结果多样：  
x ciphertexts -> x/4 ciphertexts   
x ciphertexts -> x/2 ciphertexts   
依赖tensor的切分策略，结果的type/shape组合在不同axis：  
[C, H, W]   H, H first + then C, C(比如channel_out)  

N=64K, S=32K
[3, 224, 224] -> [64, 224, 224]  // H  
[64, 112, 112]-> [128, 56, 56]   // C

抽象到一个pass来分析，reshard分解为分析和Pure lowering。
AllGather( axis)

Redistribution:
[64][112, 112] @ 64 ciphertexts
-> [32][2, 112, 112] @ 32 ciphertexts

Distribution on ciphertext mesh (implicit replicate in x axis):

| mesh[x,0,0]   | mesh[x,1,0] | mesh[x,2,0] | mesh[x,3,0] | ... | mesh[x,64,0] |
| ------------- | ----------- | ----------- | ----------- | --- | ------------ |
| D[0][112x112] | D[1]        | D[2]        | D[3]        | ... | D[63]        |

->

| mesh[x,0,0]     | mesh[x,1,0] | mesh[x,2,0] | mesh[x,3,0] | ... | mesh[x,31,0] |
| --------------- | ----------- | ----------- | ----------- | --- | ------------ |
| D[0][2x112x112] | D[1]        | D[2]        | D[3]        | ... | D[31]        |


2）上下op衔接：
ResNet18:  比较规整的channel scale up and spatial scale down
ResNet50: channel_in/out变化。input no shard，or output no shard。 (one ciphertext)
N=65536, Slots=32768

[2048,7,7] size=100352   
-> conv 1x1 -> [512,7,7] size=25088   
-> conv 3x3 -> [512,7,7] size=25088 
-> conv 1x1 -> [2048,7,7]  
-> GAP -> [2048] 

两种策略：
1-默认tensor的分布是1 ciphertext, [512,7,7]。 Loader和storer改变。default "all replication"
2-显示的处理为shard[1][512,7,7]


1) Transformation
Two type of shard transformation.
EltwiseShard:
  Mesh
  block:
      A = B + C // elementwise operation

ReduceShard:
  Mesh
  rdim=x
  indexera
  indexerb
  indexerc
  block:
      A = B @ C  // reduce_add in x dimension 

New_eltwise_shard(... body) and New_reduce_shard(... body) are used to generate corresponding structure.
Each op provides a shard_body stmt_list. 
sharding_loader and sharding_storer is for the access of shard_data.
TODO: For some ops without attrs, it can be simpler.

Formal Type Lowering: New function with the new formal type (sharding). record the mapping between original formal and new formal.
TODO: remove signature modification.


All these handlers are wrapped in sharding_opt.cxx.

class ARRAY_SHARDING describes the distributions of the input/output tensors across computation/ciphertext mesh.
```
New_sharding_type(): Actually sharding_type is a vector.
Split_array(): Split a tensor to a vector of tensor uniformly.
New_sharding_loader(): access a sharding element.
New_sharding_store(): assign a sharding element.
New_sharding_update_store(): add a sharding element and self update.
```

## Debug


## Conv Example
1) Sharding strategy analysis: how to partition each dimension of convolution.
2) Data partition: split weight/bias according to the sharding strategy.
3) Computation arrangement: a 2-d computation mesh. A possible arrangement:

| input_sharding[0]       | input_sharding[1]       |
| ----------------------- | ----------------------- |
| \*weight_sharding[0][0] | \*weight_sharding[1][0] |
| \*weight_sharding[0][1] | \*weight_sharding[1][1] |
| reduce-add              | reduce-add              |
| =                       | =                       |
| output_sharding[0]      | output_sharding[1]      |


