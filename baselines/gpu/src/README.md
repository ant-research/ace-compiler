# Phantom CKKS Library

This library provides a C++ implementation of the CKKS (Cheon-Kim-Kim-Song) homomorphic encryption scheme, with a focus on efficient evaluation of arithmetic operations on encrypted data.

## Main Components

### Encoder

Handles encoding and decoding of data for the CKKS scheme.

- Encoding of vectors and single values
- Decoding of plaintexts
- Slot count management

### Encryptor

Manages the encryption process.

- Asymmetric encryption of plaintexts

### Evaluator

Performs operations on encrypted data.

- Modulus switching and rescaling
- Relinearization
- Arithmetic operations (addition, multiplication, subtraction)
- Rotation and conjugation
- NTT transformations
- Error reduction operations

### Decryptor

Handles decryption and key generation.

- Decryption of ciphertexts
- Galois key generation

### CKKSEvaluator

High-level evaluator that combines the functionalities of the above components.

- Initialization with context and keys
- Helper functions for vector initialization and printing
- Advanced operations:
  - Sign function evaluation
  - Inverse square root
  - Exponential function
  - Inverse function
- Metric calculation (Mean Absolute Error)

## Key Features

- Efficient implementation of CKKS scheme
- Support for complex arithmetic on encrypted data
- Modular design allowing for easy integration and extension
- Comprehensive set of homomorphic operations
- Error reduction techniques for improved precision

