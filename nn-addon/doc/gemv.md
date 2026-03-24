# GEMV Design

The GEMV (or MVM) operation $C[n] = B[n,k] \times A[k]$ computes a matrix-vector product, where $B \in \mathbb{R}^{n \times k}$ 
is a plaintext matrix (representing the neural network parameters) and $A \in \mathbb{R}^k$ is an encrypted input 
vector in the CKKS scheme.  

A total of $2\times n \times k$ FLOPs and $n + n\times k + k$ byte accesses are needed to compute the product. 
However, CKKS only supports add/mul/rotate operations. To achieve efficient encrypted computation, we desing 
IMRA pattern to map GEMV operation sto CKKS by strategically leveraging these restricted operations while 
minimizing rotation overhead.

## IMRA pattern (Iterative Multiply-Rotate Accumulation)

|    concepts   |   Explaination                                   |
|:-------------:|:------------------------------------------------:|
|  n and k      | Shape of B                                       |
|  np and kp    | Padding of n and k for Shape Divisable           |
|  nd and kd    | Shape of IMRA packing(B)                         |

### Code workflow
1) B padding: Padding $B[n,k]$ to Shape Divisable $Bdiv[np,kp]$ by Compute_pad_gemm.
For certain shapes of $[n,k]$, padding can lead to underutilized slots. But it helps rotation reduction!
Example: [1000, 512] -> [1024, 512].
2) B Transformation to M for original IMRA pattern: Data_transform_irma
3) Compile-time IMRA parameter optimization by rotation-aware cost model Imra_cost_model. 
And M is transformed according the new parameters.  Horizental batching uses interleaved block distribution.
4) MetaKernel template by New_gemm_metakernel_fast.

IMRA Parameters:
- `bs`: block size, which is the basic unit of a Metakernel.
- `Pb`: Number of blocks, $Pb=nd/bs$.
- 'gs': Total MetaKernels for the GEMV, $gs=Pb/Ps$. 
- `Ps`: Horizental batching of blocks, $Ps$ blocks are merged by interleaved “gs” block distribution.
- `sf`: Shift buffer size for cyclic rotations of each merged block.
- 'shift': for CKKS rotate of each MetaKernel.

## Tracing in the phase
Certain critical data needs to be recorded via tracing to facilitate debugging.
 - Change of shapes due to B transformation.
 - IMRA parameter search and the best parameters: num_search for the number of searchable paramerters.
 - "comment" in the generated code for better readability.
 - Rotations tracing by comment.


Generated GEMV code with good "comments":
 ```
// BlockingRot replicate=9
for ...
// BlockingRot HRot=bs=8
for (bs=0 to 8)
// IMRA Metakernel: gs=8
for (gs=0 to 8) ...
// gemm result reduce->Ps=8
for (r=0 to 8)
// gemm result reduce->(kp/np)=1
 ```

## Performance and IMRA parameters: N=32K Slots=16K
| GEMV B        |      bs     |      pb     |     Ps      |      sf     |   A rep     |
|:-------------:|:-----------:|:-----------:|:-----------:|:-----------:|:-----------:|
| [512, 512]    |    8        |    64       |   8         |    64       |    9        |
| [4096, 4096]  |    32       |    128      |   4         |    1024     |    5        |
| [1000,512]    |    8        |    64       |   8         |    64       |    17       |


