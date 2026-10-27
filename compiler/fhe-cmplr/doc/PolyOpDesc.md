 ## POLY Operators V1 - For CPU emitting
| Category                | Operator                                    |
| ------------------- | --------------------------------------- |
| Memory Management   | [POLY.alloc](#POLY.alloc)               |
| Memory Management   | [POLY.free](#POLY.free)                 |
| Arithmetic          | [POLY.ntt](#POLY.ntt)                   |
| Arithmetic          | [POLY.intt](#POLY.intt)                 |
| Arithmetic          | [POLY.add](#POLY.add)                   |
| Arithmetic          | [POLY.sub](#POLY.sub)                   |
| Arithmetic          | [POLY.mul](#POLY.mul)                   |
| Arithmetic          | [POLY.add_ext](#POLY.add_ext)           |
| Arithmetic          | [POLY.sub_ext](#POLY.sub_ext)           |
| Arithmetic          | [POLY.mul_ext](#POLY.mul_ext)           |
| Arithmetic          | [POLY.rescale](#POLY.rescale)           |
| Arithmetic          | [POLY.rotate](#POLY.rotate)             |
| Arithmetic          | [POLY.decomp](#POLY.decomp)             |
| Arithmetic          | [POLY.mod_up](#POLY.mod_up)             |
| Arithmetic          | [POLY.decomp_modup](#POLY.decomp_modup) |
| Arithmetic          | [POLY.auto_order](#POLY.auto_order)     |
| Arithmetic          | [POLY.mod_down](#POLY.mod_down)         |
| Arithmetic          | [POLY.modswitch](#POLY.modswitch)       |
| Property            | [POLY.coeffs](#POLY.coeffs)             |
| Property            | [POLY.set_coeffs](#POLY.set_coeffs)     |
| Property            | [POLY.level](#POLY.level)               |
| Property            | [POLY.num_p](#POLY.num_p)               |
| Property            | [POLY.num_alloc](#POLY.num_alloc)       |
| Property            | [POLY.num_decomp](#POLY.num_decomp)     |
| Property            | [POLY.degree](#POLY.degree)             |
| Property            | [POLY.q_modulus](#POLY.q_modulus)       |
| Property            | [POLY.p_modulus](#POLY.p_modulus)       |
| Property            | [POLY.swk](#POLY.swk)                   |
| Property            | [POLY.pk0_at](#POLY.pk0_at)             |
| Property            | [POLY.pk1_at](#POLY.pk1_at)             |
| Hardware Arithmetic | [POLY.hw_ntt](#POLY.hw_ntt)             |
| Hardware Arithmetic | [POLY.hw_intt](#POLY.hw_intt)           |
| Hardware Arithmetic | [POLY.hw_modadd](#POLY.hw_modadd)       |
| Hardware Arithmetic | [POLY.hw_modsub](#POLY.hw_modsub)       |
| Hardware Arithmetic | [POLY.hw_modmul](#POLY.hw_modmul)       |
| Hardware Arithmetic | [POLY.hw_rotate](#POLY.hw_rotate)       |

### **POLY.alloc**
* **Summary**
  * Allocate POLYNOMIAL with specified level
* **Input**
  * **A** - **Int**: ring degree
  * **B** - **Int**: level
  * **C** - **Bool**: contains p primes
* **Output**
  * **D** - **POLYNOMIAL**: allocated POLYNOMIAL

### **POLY.free**
* **Summary**
  * Free POLYNOMIAL memory
* **Input**
  * **A** - **POLYNOMIAL**: input POLYNOMIAL to be freed
* **Output**
  * None

### **POLY.ntt**
* **Summary**
  * Convert specified POLYNOMIAL from coefficient format to NTT format
* **Input**
  * **A** - **POLYNOMIAL**: input POLYNOMIAL with coefficient format
* **Output**
  * **B** - **POLYNOMIAL**: output POLYNOMIAL with NTT format

### **POLY.intt**
* **Summary**
  * Convert specified POLYNOMIAL from NTT format to coefficient format
* **Input**
  * **A** - **POLYNOMIAL**: input POLYNOMIAL with NTT format
* **Output**
  * **B** - **POLYNOMIAL**: output POLYNOMIAL with coefficient format

### **POLY.add**
* **Summary**
  * Perform POLYNOMIAL element-wise add under RNS.  $C_i=(A_i + B_i) \% q_i$
* **Input**
  * **A** - **POLYNOMIAL**: first operand
  * **B** - **POLYNOMIAL**: second operand
* **Output**
  * **C** - **POLYNOMIAL**: result

### **POLY.sub**
* **Summary**
  * Perform POLYNOMIAL element-wise subtract under RNS.  $C_i=(A_i - B_i) \% q_i$
* **Input**
  * **A** - **POLYNOMIAL**: first operand
  * **B** - **POLYNOMIAL**: second operand
* **Output**
  * **C** - **POLYNOMIAL**: result

### **POLY.mul**
* **Summary**
  * Perform POLYNOMIAL element-wise multiply under RNS.  $C_i=(A_i * B_i) \% q_i$
* **Input**
  * **A** - **POLYNOMIAL**: first operand
  * **B** - **POLYNOMIAL**: second operand
* **Output**
  * **C** - **POLYNOMIAL**: result

### **POLY.add_ext**
* **Summary**
  * Perform POLYNOMIAL element-wise add under RNS, including P part.  $C_i=(A_i + B_i) \% q_i, C_{l+j} = (A_{l+j} + B_{l+j}) \% p_j$,  $0 \le i \lt l$ (q number), $0 \le j \lt k$ (p number)
* **Input**
  * **A** - **POLYNOMIAL**: first operand
  * **B** - **POLYNOMIAL**: second operand
* **Output**
  * **C** - **POLYNOMIAL**: result

### **POLY.sub_ext**
* **Summary**
  * Perform POLYNOMIAL element-wise subtract under RNS, including P part.  $C_i=(A_i - B_i) \% q_i, C_{l+j} = (A_{l+j} - B_{l+j}) \% p_j$,  $0 \le i \lt l$ (q number), $0 \le j \lt k$ (p number)
* **Input**
  * **A** - **POLYNOMIAL**: first operand
  * **B** - **POLYNOMIAL**: second operand
* **Output**
  * **C** - **POLYNOMIAL**: result

### **POLY.mul_ext**
* **Summary**
  * Perform POLYNOMIAL element-wise multiply under RNS, including P part.  $C_i=(A_i * B_i) \% q_i, C_{l+j} = (A_{l+j} * B_{l+j}) \% p_j$,  $0 \le i \lt l$ (q number), $0 \le j \lt k$ (p number)
* **Input**
  * **A** - **POLYNOMIAL**: first operand
  * **B** - **POLYNOMIAL**: second operand
* **Output**
  * **C** - **POLYNOMIAL**: result

### **POLY.rescale**
* **Summary**
  * Correspond to CKKS.rescale, lowered to two POLYNOMIAL rescale. 
* **Input**
  * **A** - **POLYNOMIAL**: first operand
* **Output**
  * **C** - **POLYNOMIAL**: result

### **POLY.rotate**
* **Summary**
  * Correspond to CKKS.rotate, lowered to two POLYNOMIAL rotate. 
* **Input**
  * **A** - **POLYNOMIAL**: POLYNOMIAL to be rotated
  * **B** - **Int**: rotate index
* **Output**
  * **C** - **POLYNOMIAL**: result

### **POLY.decomp**
* **Summary**
  * CKKS hybrid keyswitch first step, decompose POLYNOMIAL at specified digit index
* **Input**
  * **A** - **POLYNOMIAL**: POLYNOMIAL to be decomposed
  * **B** - **Int**: digit index
* **Output**
  * **C** - **POLYNOMIAL**: result, $C_i = A_{B * part\_size + i}$

### **POLY.mod_up**
* **Summary**
  * CKKS hybrid keyswitch second step, raise POLYNOMIAL RNS modulus from {$q_i,...q_j$} to {$q_0....q_l,...p_0...p_k$} at specified digit index
* **Input**
  * **A** - **POLYNOMIAL**: POLYNOMIAL to be mod_up
  * **B** - **Int**: digit index
* **Output**
  * **C** - **POLYNOMIAL**: result

### **POLY.decomp_modup**
* **Summary**
  * CKKS hybrid keyswitch first/second step, fused operation for POLY.decomp and POLY.modup
* **Input**
  * **A** - **POLYNOMIAL**: POLYNOMIAL to be decomp_modup
  * **B** - **Int**: digit index
* **Output**
  * **C** - **POLYNOMIAL**: result

### **POLY.mod_down**
* **Summary**
  * CKKS hybrid keyswitch last step, reduce POLYNOMIAL RNS modulus from {$q_0...q_l,p_0...p_k$} to {$q_0...q_l$}
* **Input**
  * **A** - **POLYNOMIAL**: POLYNOMIAL to be mod_down
* **Output**
  * **B** - **POLYNOMIAL**: result

### **POLY.auto_order**
* **Summary**
  * Calcuate automorphism order for specified rotation index
* **Input**
  * **A** - **Int**: rotation index
* **Output**
  * **B** - **Int[N]**: size N array of integers records the order of automorphism transformation

### **POLY.modswitch**
* **Summary**
  * Lowered from CKKS.modswitch, drop last modulus
* **Input**
  * **A** - **POLYNOMIAL**
* **Output**
  * **B** - **POLYNOMIAL**: result 

### **POLY.coeffs**
* **Summary**
  * Get POLYNOMIAL coefficient data address at specified level
* **Input**
  * **A** - **POLYNOMIAL**
  * **B** - **INT**: level
  * **C** - **INT**: ring degree
* **Output**
  * **D** - **INT \***: result: coeffcient start address at level B

### **POLY.set_coeffs**
* **Summary**
  * Set POLYNOMIAL coefficient data at specified level
* **Input**
  * **A** - **POLYNOMIAL**: input
  * **B** - **INT**: level
  * **C** - **INT**: ring degree
  * **D** - **INT \***: coeffcient data
* **Output**
  * None

### **POLY.level**
* **Summary**
  * Get POLYNOMIAL level
* **Input**
  * **A** - **POLYNOMIAL**: input
* **Output**
  * **B** - **INT**: level

### **POLY.num_p**
* **Summary**
  * Get POLYNOMIAL number of p primes
* **Input**
  * **A** - **POLYNOMIAL**: input
* **Output**
  * **B** - **INT**: number of p primes

### **POLY.num_alloc**
* **Summary**
  * Get POLYNOMIAL number of allocated primes including q and p primes
* **Input**
  * **A** - **POLYNOMIAL**: input
* **Output**
  * **B** - **INT**: number of allocated primes

### **POLY.num_decomp**
* **Summary**
  * Get the number of groups for POLYNOMIAL decomposition
* **Input**
  * **A** - **POLYNOMIAL**: input
* **Output**
  * **B** - **INT**: number of decomposition

### **POLY.degree**
* **Summary**
  * Get the size of POLYNOMIAL ring degree
* **Input**
  * **A** - **POLYNOMIAL**: input
* **Output**
  * **B** - **INT**: ring degree

### **POLY.q_modulus**
* **Summary**
  * Get RNS modulus for q primes
* **Input**
  * None
* **Output**
  * **A** - **MODULUS \***: Q modulus head

### **POLY.p_modulus**
* **Summary**
  * Get RNS modulus for p primes
* **Input**
  * None
* **Output**
  * **A** - **MODULUS \***: P modulus head

### **POLY.swk**
* **Summary**
  * Get switch key for rotation or relinearization key
* **Input**
  * **A** - **BOOL**: is rotation key
  * **B** - **INT**: for rotation key, specify the rotation index
* **Output**
  * **C** - **SWITCH_KEY \***: the pointer of switch key

### **POLY.pk0_at**
* **Summary**
  * Get the first POLYNOMIAL of switch key's public key pair at specified digit index
* **Input**
  * **A** - **SWITCH_KEY**: switch key
  * **B** - **INT**: digit index
* **Output**
  * **C** - **POLYNOMAIL**: result

### **POLY.pk1_at**
* **Summary**
  * Get the second POLYNOMIAL of switch key's public key pair at specified digit index
* **Input**
  * **A** - **SWITCH_KEY**: switch key
  * **B** - **INT**: digit index
* **Output**
  * **C** - **POLYNOMAIL**: result

### **POLY.hw_ntt**
* **Summary**
  * Performs hardware NTT
* **Input**
  * **A** - **INT \***: data
  * **B** - **MODULUS \***: modulus
* **Output**
  * **C** - **INT \***: result after NTT transformation

### **POLY.hw_intt**
* **Summary**
  * Performs hardware INTT
* **Input**
  * **A** - **INT \***: data
  * **B** - **MODULUS \***: modulus
* **Output**
  * **C** - **INT \***: result after NTT transformation

### **POLY.hw_modadd**
* **Summary**
  * Performs hardware modulus add at specified modulus
* **Input**
  * **A** - **INT \***: first operand
  * **B** - **INT \***: second operand
  * **C** - **MODULUS \***: modulus
* **Output**
  * **D** - **INT \***: result

### **POLY.hw_modmul**
* **Summary**
  * Performs hardware modulus multiply at specified modulus
* **Input**
  * **A** - **INT \***: first operand
  * **B** - **INT \***: second operand
  * **C** - **MODULUS \***: modulus
* **Output**
  * **D** - **INT \***: result

### **POLY.hw_rotate**
* **Summary**
  * Performs hardware rotation/automorphism at specified modulus
* **Input**
  * **A** - **INT \***: first operand
  * **B** - **INT \***: automorphism order
  * **C** - **MODULUS \***: modulus
* **Output**
  * **D** - **INT \***: result

## POLY Operators V2
POLY V2 reconstruct the POLY-level operators into two-layer : **HPOLY**(High-Level POLY) and **LPOLY**(Low-level POLY). This layered architecture is designed to expose additional optimization opportunities and to better target for HPU. The full functionality is still under development.

HPOLY is designed to abstract operations for RNS_POLY granularity, which is target independent.

LPOLY is desinged to abstract operations for single POLY  granularity, which is target dependant.

A brief type lowering from CKKS->HPOLY->LPOLY is listed as below
![rnspoly.png](https://intranetproxy.alipay.com/skylark/lark/0/2025/png/92656830/1736234083282-49d624a4-bb6a-4100-b704-36ce3a21397f.png) 

### HPOLY Operators
  | Category   | Operator                                      |
  | ---------- | --------------------------------------------- |
  | Arithmetic | [HPOLY.extract](#HPOLY.extract)               |
  | Arithmetic | [HPOLY.concat](#HPOLY.concat)                 |
  | Arithmetic | [HPOLY.add](#HPOLY.add)                       |
  | Arithmetic | [HPOLY.add_ext](#HPOLY.add_ext)               |
  | Arithmetic | [HPOLY.sub](#HPOLY.sub)                       |
  | Arithmetic | [HPOLY.sub_ext](#HPOLY.sub_ext)               |
  | Arithmetic | [HPOLY.mul](#HPOLY.mul)                       |
  | Arithmetic | [HPOLY.mul_ext](#HPOLY.mul_ext)               |
  | Arithmetic | [HPOLY.mac](#HPOLY.mac)                       |
  | Arithmetic | [HPOLY.mac_ext](#HPOLY.mac_ext)               |
  | Arithmetic | [HPOLY.rotate](#HPOLY.rotate)                 |
  | Arithmetic | [HPOLY.ntt](#HPOLY.ntt)                       |
  | Arithmetic | [HPOLY.intt](#HPOLY.intt)                     |
  | Arithmetic | [HPOLY.modswitch](#HPOLY.modswitch)           |
  | Arithmetic | [HPOLY.bconv](#HPOLY.bconv)                   |
  | Arithmetic | [HPOLY.bswitch](#HPOLY.bswitch)               |
  | Arithmetic | [HPOLY.extend](#HPOLY.extend)                 |
  | Arithmetic | [HPOLY.**rescale***](#HPOLY.rescale)          |
  | Arithmetic | [HPOLY.**precom***](#HPOLY.**precom***)       |
  | Arithmetic | [HPOLY.**dot_prod***](#HPOLY.**dotprod***)     |
  | Arithmetic | [HPOLY.**mod_down***](#HPOLY.**moddown***)     |
  | Arithmetic | [HPOLY.**raise_mod***](#HPOLY.**raise_mod***) |

  Note: Operator with "*" will be continously lowered to HPOLY basic operators(with out *)

### **HPOLY.extract**
* **Summary**
  * Extract RNS_POLY from A start from index B to C
* **Input**
  * **A** - **RNS_POLY**: input
  * **B** - **INT**: start index
  * **C** - **INT**: end index
* **Output**
  * **D** - **RNS_POLY**: result

### **HPOLY.concat**
* **Summary**
  * Concat A and B
* **Input**
  * **A** - **RNS_POLY**: first operand
  * **B** - **INT**: second operand
* **Output**
  * **C** - **RNS_POLY**: result

### **HPOLY.add**
* **Summary**
  * Perform RNS_POLY element-wise add at Q RNS basis
* **Input**
  * **A** - **RNS_POLY**: first operand
  * **B** - **RNS_POLY**: second operand
* **Output**
  * **C** - **RNS_POLY**: result

### **HPOLY.add_ext**
* **Summary**
  * Perform RNS_POLY element-wise add at QP RNS basis
* **Input**
  * **A** - **RNS_POLY**: first operand
  * **B** - **RNS_POLY**: second operand
* **Output**
  * **C** - **RNS_POLY**: result

### **HPOLY.sub**
* **Summary**
  * Perform RNS_POLY element-wise sub at Q RNS basis
* **Input**
  * **A** - **RNS_POLY**: first operand
  * **B** - **RNS_POLY**: second operand
* **Output**
  * **C** - **RNS_POLY**: result

### **HPOLY.sub_ext**
* **Summary**
  * Perform RNS_POLY element-wise sub at QP RNS basis
* **Input**
  * **A** - **RNS_POLY**: first operand
  * **B** - **RNS_POLY**: second operand
* **Output**
  * **C** - **RNS_POLY**: result

### **HPOLY.mul**
* **Summary**
  * Perform RNS_POLY element-wise multiply at Q RNS basis
* **Input**
  * **A** - **RNS_POLY**: first operand
  * **B** - **RNS_POLY**: second operand
* **Output**
  * **C** - **RNS_POLY**: result

### **HPOLY.mul_ext**
* **Summary**
  * Perform RNS_POLY element-wise multiply at QP RNS basis
* **Input**
  * **A** - **RNS_POLY**: first operand
  * **B** - **RNS_POLY**: second operand
* **Output**
  * **C** - **RNS_POLY**: result

### **HPOLY.mac**
* **Summary**
  * Perform RNS_POLY element-wise multiply add at Q RNS basis $D_i = (A_i + B_i * C_i)$
* **Input**
  * **A** - **RNS_POLY**: first operand
  * **B** - **RNS_POLY**: second operand
  * **C** - **RNS_POLY**: third operand
* **Output**
  * **D** - **RNS_POLY**: result

### **HPOLY.mac_ext**
* **Summary**
  * Perform RNS_POLY element-wise multiply add at QP RNS basis $D_i = (A_i + B_i * C_i)$
* **Input**
  * **A** - **RNS_POLY**: first operand
  * **B** - **RNS_POLY**: second operand
  * **C** - **RNS_POLY**: third operand
* **Output**
  * **D** - **RNS_POLY**: result

### **HPOLY.rotate**
* **Summary**
  * Performs automorphism transform for A with order B at Q RNS_basis
* **Input**
  * **A** - **RNS_POLY**: first operand
  * **B** - **INT[]**: automorphism order
* **Output**
  * **C** - **RNS_POLY**: result

### **HPOLY.ntt**
* **Summary**
  * Performs NTT transformation
* **Input**
  * **A** - **RNS_POLY**: input with Coefficient format
* **Output**
  * **B** - **RNS_POLY**: result with NTT format

### **HPOLY.intt**
* **Summary**
  * Performs INTT transformation
* **Input**
  * **A** - **RNS_POLY**: input with NTT format
* **Output**
  * **B** - **RNS_POLY**: result with Coefficient format

### **HPOLY.modswitch**
* **Summary**
  * Correspond to CKKS.modswitch, lowered to two RNS_POLY modswitch, drop the last RNS modulus.
* **Input**
  * **A** - **RNS_POLY**: input
* **Output**
  * **B** - **RNS_POLY**: result

### **HPOLY.rescale** (HPOLY phaseI)
* **Summary**
  * Correspond to CKKS.rescale, lowered to two RNS_POLY rescale. 
* **Input**
  * **A** - **RNS_POLY**: input
* **Output**
  * **B** - **RNS_POLY**: result


### **HPOLY.precom** (HPOLY phaseI)
* **Summary**
  * CKKS hybrid keyswitch decompose and mod_up step
* **Input**
  * **A** - **RNS_POLY**: input
* **Output**
  * **B** - **RNS_POLY[]**: result, a group of RNS_POLY with QP RNS basis

### **HPOLY.innpro/dotprod** (HPOLY phaseI)
* **Summary**
  * CKKS hybrid keyswitch inner product with switch key
* **Input**
  * **A** - **RNS_POLY[]**: array of RNS_POLY
  * **B** - **RNS_POLY[]**: switch key
* **Output**
  * **C** - **RNS_POLY**: result, $C = \sum_{i=0}^{A.size-1}A_i * B_i$

### **HPOLY.moddown** (HPOLY phaseI)
* **Summary**
  * CKKS hybrid keyswitch last step, reduce RNS modulus from {$q_0...q_l,p_0...p_k$} to {$q_0...q_l$}
* **Input**
  * **A** - **RNS_POLY**: input, RNS modulus  {$q_0...q_l,p_0...p_k$}
* **Output**
  * **B** - **RNS_POLY**: result, RNS modulus {$q_0...q_l$}

### **HPOLY.extend** (HPOLY phaseI) - not used yet
* **Summary**
  * Extend RNS modulus from {$q_0...q_l$} to {$q_0...q_l,p_0...p_k$} by multiplying with Q portion with $P \% q_j$, and set P portion coefficients with zero
* **Input**
  * **A** - **RNS_POLY**: input, RNS modulus  {$q_0...q_l$}
* **Output**
  * **B** - **RNS_POLY**: result, RNS modulus {$q_0...q_l,p_0...p_k$}
 
### **HPOLY.raise_mod** (HPOLY phaseI)
* **Summary**
  *  The first step of Bootstrap, which raise the modulus of input of input
     level to the required target level
* **Input**
  * **A** - **RNS_POLY** input
  * **B** - **INT** raised level
* **Output**
  * **C** - **RNS_POLY** output with raised level

### **HPOLY.bconv** ( HPOLY phaseII)
* **Summary**
  * Convert input RNS moduls from {$q_0...q_l$} to specified RNS ranges {C...D}
* **Input**
  * **A** - **RNS_POLY**: input, RNS modulus  {$q_0...q_l$}
  * **B** - constant array QLHMODP
  * **C** - new RNS basis start
  * **D** - new RNS basis end

### **HPOLY.bswitch** (HPOLY phaseII)
* **Summary**
  * Switch input RNS moduls(only one modulus) to specified RNS ranges {C...D}, used in rescale and raisemod
* **Input**
  * **A** - **RNS_POLY**: input, RNS modulus  {$q$}
  * **B** - constant array QLHMODQ
  * **C** - new RNS basis start
  * **D** - new RNS basis end
* **Output**
  * **E** - **RNS_POLY**, output with RNS modulus {C,D}



### LPOLY Operators
  | Category   | Operator                        | HPU                |
  | ---------- | ------------------------------- | ------------------ |
  | Memory     | [LPOLY.alloc_n](#LPOLY.alloc_n) |                    |
  | Memory     | [LPOLY.alloc](#LPOLY.alloc)     |                    |
  | Memory     | [LPOLY.free](#LPOLY.free)       |                    |
  | Access     | [LPOLY.ld](#LPOLY.ld)           |                    |
  | Access     | [LPOLY.st](#LPOLY.st)           |                    |
  | Arithmetic | [LPOLY.add](#LPOLY.add)         | :white_check_mark: |
  | Arithmetic | [LPOLY.sub](#LPOLY.sub)         | :white_check_mark: |
  | Arithmetic | [LPOLY.mul](#LPOLY.mul)         | :white_check_mark: |
  | Arithmetic | [LPOLY.mac](#LPOLY.mac)         | :white_check_mark: |
  | Arithmetic | [LPOLY.autou](#LPOLY.autou)     | :white_check_mark: |
  | Arithmetic | [LPOLY.ntt](#LPOLY.ntt)         | :white_check_mark: |
  | Arithmetic | [LPOLY.intt](#LPOLY.intt)       | :white_check_mark: |
  | Arithmetic | [LPOLY.innpro](#LPOLY.innpro)   | :white_check_mark: |
  | Arithmetic | [LPOLY.bconv](#LPOLY.bconv)     | :white_check_mark: |
  | Property   | [LPOLY.ldmod](#LPOLY.ldmod)     | :white_check_mark: |
  | Property   | [LPOLY.ldmod3s](#LPOLY.ldmod3s) | :white_check_mark: |

### **LPOLY.alloc_n**
* **Summary**
  * allocate an array of POLY with size A
* **Input**
  * **A** - **INT**: allocation size
* **Output**
  * **B** - **POLY[]**: result, array of POLY with size A

### **LPOLY.alloc**
* **Summary**
  * allocate a single POLY
* **Input**
  * None
* **Output**
  * **A** - **POLY**: result

### **LPOLY.free**
* **Summary**
  * free the memory of operand
* **Input**
  * **A** - **POLY[]|POLY**
* **Output**
  * None

### **LPOLY.ld**
* **Summary**
  * Load an LPOLY at specified index
* **Input**
  * **A** - **POLY|RNS_POLY|Ciphertext|Ciphertext3|Plaintext|POLY Array|Any POLY Aggregates**: first operand
  * **B** - **index**: the index in A, if POLY type, the index is always 0
* **Output**
  * **D** - **POLY**: result

### **LPOLY.st**
* **Summary**
  * Store to an LPOLY object at specified index
* **Input**
  * **A** - **POLY|RNS_POLY|Ciphertext|Ciphertext3|Plaintext|POLY Array|Any POLY Aggregates**: first operand
  * **B** - **POLY**: the index in A, if POLY type, the index is always 0
* **Output**
  * None

### **LPOLY.add**
* **Summary**
  * Perform element-wise mod add with specified modulus
* **Input**
  * **A** - **POLY**: first operand
  * **B** - **POLY**: second operand
  * **C** - **CRT_PRIME|INT**: modulus (CRT_PRIME for CPU, INT for HPU)
* **Output**
  * **D** - **POLY**: result

### **LPOLY.sub**
* **Summary**
  * Perform element-wise mod subtract with specified modulus
* **Input**
  * **A** - **POLY**: first operand
  * **B** - **POLY**: second operand
  * **C** - **CRT_PRIME|INT**: modulus (CRT_PRIME for CPU, INT for HPU)
* **Output**
  * **D** - **POLY**: result

### **LPOLY.mul**
* **Summary**
  * Perform element-wise mod multiply with specified modulus
* **Input**
  * **A** - **POLY**: first operand
  * **B** - **POLY**: second operand
  * **C** - **CRT_PRIME|INT**: modulus (CRT_PRIME for CPU, INT for HPU)
* **Output**
  * **D** - **POLY**: result

### **LPOLY.mac**
* **Summary**
  * Perform element-wise mod multiply add with specified modulus
* **Input**
  * **A** - **POLY**: first operand
  * **B** - **POLY**: second operand
  * **C** - **CRT_PRIME|INT**: modulus (CRT_PRIME for CPU, INT for HPU)
* **Output**
  * **D** - **POLY**: result

### **LPOLY.autou**
* **Summary**
  * Perform automorphism transformation with specified modulus
* **Input**
  * **A** - **POLY**: first operand
  * **B** - **INT[]|INT**: automorphism order, INT[] for CPU, INT for HPU
  * **C** - **CRT_PRIME|INT**: modulus (CRT_PRIME for CPU, INT for HPU)
* **Output**
  * **D** - **POLY**: result

### **LPOLY.ntt**
* **Summary**
  * Perform NTT transformation with specified modulus
* **Input**
  * **A** - **POLY**: first operand
  * **B** - **CRT_PRIME|INT**: modulus (CRT_PRIME for CPU, INT for HPU)
* **Output**
  * **C** - **POLY**: result

### **LPOLY.intt**
* **Summary**
  * Perform INTT transformation with specified modulus
* **Input**
  * **A** - **POLY**: first operand
  * **B** - **CRT_PRIME|INT**: modulus (CRT_PRIME for CPU, INT for HPU)
* **Output**
  * **C** - **POLY**: result

### **LPOLY.innpro**
* **Summary**
  * Perform inner product with specified modulus
* **Input**
  * **A** - **POLY[]**: first operand
  * **B** - **POLY[]**: second operand
  * **C** - **CRT_PRIME|INT**: modulus (CRT_PRIME for CPU, INT for HPU)
* **Output**
  * **D** - **POLY**: result, $D = \sum_{i=0}^{A.size-1}(A_i * B_i) \% C$

### **LPOLY.bconv - batch op not used yet**
* **Summary**
  * HPU only, mapping to HPU fhe.bconv instruction 
* **Input**
  * **A** - **POLY[]**: first operand
  * **B** - **INT[]**: second operand
  * **C** - **INT**: modulus
* **Output**
  * **D** - **POLY**: result, $D = \sum_{i=0}^{A.size-1}(A_i * B_i) \% C$

### **LPOLY.ldmod**
* **Summary**
  * Get the modulus value at specified RNS index
* **Input**
  * **A** - **INT**: rns index
* **Output**
  * **B** - **CRT_PRIME|INT**: modulus (CRT_PRIME for CPU, INT for HPU)

### **LPOLY.ldmod3s**
* **Summary**
  * HPU only, Get the 3 modulus(QMUK) value at specified RNS index
* **Input**
  * **A** - **INT**: rns index
* **Output**
  * **B** - **INT**: modulus