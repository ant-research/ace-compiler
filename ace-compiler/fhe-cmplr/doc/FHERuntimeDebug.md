# Compiler and Runtime Support for FHE Runtime Debuggability

**Important**: Markdown file (.md) is the master copy. PDF file (.pdf) is exported from markdown file only for review.

## Revision History

|Version|Author     |Date      |Description|
|-------|-----------|----------|-----------|
|0.1    |jianxin.lai|2023.10.31|Initial version.|

## Terms
|Term         |Description |
|-------------|------------|
|*message*    |usually *message* is a vector/matrix/tensor with primitive types. message can be encoded into plaintext or decode from plaintext. |
|*plaintext*  |usually *plaintext* is a polynomial. plaintext is encoded from message, or decrypt from cipher text. |
|*ciphertext* |usually *ciphertext* is a pair of polynomials. ciphertext is encrypted from a plaintext. |

## Introduction
Runtime debuggability is used to help triaging runtime failures, especially for output difference error. The basically idea is to operate on message and plaintext/ciphertext respectively and compare their result. If no error in compiler or runtime, the result should be the same (precise FHE) or within a small epsilon (approximate FHE).
```c++
// assume x is a ciphertext. for precise FHE
func_message(decode(decrypt(x))) == decode(decrypt(func_cipher(x)))

// assume x is a ciphertext. for approximate FHE
fabs(func_message(decode(decrypt(x))) - decode(decrypt(func_cipher(x)))) < epsilon
```

To reduce work in compiler, func_message can be implemented in Runtime Library (rtlib).

## Compiler Support
### Validate Operator
VALIDATE is a statement, which takes 4 kids for plain/cipher result, message result, length and epsilon respectively. Kid 0 is result in ciphertext. Kid 1 is result from message op which decodes/decrypts operand before perform the operation. Kid 2 is the length of element to validate. Kid 3 is the max error allowed which is represented by 10^epsilon.

```c++
STORE res
  OP
    LOAD opnd
VALIDATE
  LOAD res
  OP_MESSAGE
    LOAD opnd
  INTCONST len
  INTCONST epsilon
```

Runtime *DOES NOT* guarantee the kids must be evaluated during the FHE program execution. So if cipher operation is put as VALIDATE's kid 0, the code may *NOT* take effective.
```c++
VALIDATE
  OP
    LOAD opnd
  OP_MESSAGE
    LOAD opnd
  INTCONST len
  INTCONST epsilon
```

VALIDATE operator can be created by CONTAINER's interface New_validate() which takes 4 parameters:
```c++
STMT_PTR New_validate(CONST_NODE_PTR cipher_res, CONST_NODE_PTR message_op,
                      CONST_NODE_PTR length, CONST_NODE_PTR epsilon, CONST_SPOS_PTR spos);
```

### OP and OP_MESSAGE
OP is the operator currently implemented in nn-addon and fhe-cmplr, which can represent a high level tensor or vector op, or a middle level HE op. OP_MESSAGE should be added to represents the operation on message. OP_MESSAGE should take the same kids as OP. For example:
```c++
STORE res
  NN.GEMM
    LOAD input
    LDC  weight
    LDC  bias
VALIDATE
  LOAD res
  NN.GEMM_MESSAGE
    LOAD input
    LDC  weight
    LDC  bias
  INTCONST len=n*c*h*w
  INTCONST epsilon=-7
```
```c++
STORE res
  VECTOR.MUL
    LOAD input
    LDC  weight
VALIDATE
  LOAD res
  VECTOR.MUL_MESSAGE
    LOAD input
    LDC  weight
  INTCONST len=vec_len
  INTCONST epsilon=-7
```
```c++
STORE res
  SIHE.ROTATE
    LOAD input
    INTCONST 1
VALIDATE
  LOAD res
  SIHE.ROTATE_MESSAGE
    LOAD input
    INTCONST 1
  INTCONST len=vec_len
  INTCONST epsilon=-7
```

## Runtime Library Support
### Validate() function
Validate() function has the prototype and pseudo implementation below:
```c++
void Validate(CIPHER cipher, double* message, size_t len, int32_t epsilon) {
  double* res = Decode(Decrypt(cipher));
  double  error = pow(10, epsilen);
  bool found_error = false;
  for (size_t i = 0; i < len; ++i) {
    if (fabs(res[i] - message[i]) > error) {
      Report_error("Validate failed at %d. %f != %f\n", i, res[i], message[i]);
      found_error = true;
      break;
    }
  }
  Free(res);
  Free(message);
  if (found_error) {
    abort();
  }
}
```

### Op_message() function
Op_message() function prototypes and implementations are defined by OP itself. For example:
```c++
double* Vector_mul_message(CIPHER* input, PLAIN* weight) {
  double* res = Alloc(sizeof(double) * weight->_slots);
  double* op0 = Decode(Decrypt(input));
  double* op1 = Decode(weight);
  for (size_t i = 0; i < weight->_slots; ++i) {
    res[i] = op0[i] * op1[i];
  }
  Free(op0);
  Free(op1);
  return res;
}
```

### Decrypt() and Decode() function
These two functions reuse existing rtlib functions.


## Memory Management
Implementation of OP_MESSAGE in rtlib should malloc memory for result and return the pointer to the result. This memory block will be freed in VALIDATE after the comparison is done. Memory block allocated by Decode/Decrypt called in OP_MESSAGE should be freed inside OP_MESSAGE implementation after the operation.
VALIDATE should free memory allocated by Decode/Decrypt called in VALIDATE and memory block returned from OP_MESSAGE after the comparison.

## Issues
### Attributes or Kids?
Some ONNX OP attributes (pad, stride, etc) actually impacts how the operation is performed. Should these ONNX OP attributes be represented by attribute or child?

### High Level Shape Info?
Tensor shape will impact how the tensor op is performed. These information is kept in type info. How this kind of type info be passed to Op_message() implemented in rtlib?

### Validate Length?
Validate length is calculated from type info. How to get this length on ciphertext/plaintext?

