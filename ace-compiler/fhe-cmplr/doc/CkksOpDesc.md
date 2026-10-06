## CKKS
| Category   | Operator                            |
| ---------- | ----------------------------------- |
| Encode     | [CKKS.encode](#CKKS.encode)         |
| Arithmetic | [CKKS.add](#CKKS.add)               |
| Arithmetic | [CKKS.sub](#CKKS.sub)               |
| Arithmetic | [CKKS.mul](#CKKS.mul)               |
| Arithmetic | [CKKS.rotate](#CKKS.rotate)         |
| Scale Management | [CKKS.rescale](#CKKS.rescale)       |
| Scale Management | [CKKS.upscale](#CKKS.upscale)       |
| Scale Management| [CKKS.modswitch](#CKKS.modswitch)   |
| Scale Management| [CKKS.relin](#CKKS.relin)           |
| Scale Management| [CKKS.bootstrap](#CKKS.bootstrap)   |
| Property   | [CKKS.scale](#CKKS.scale)           |
| Property   | [CKKS.level](#CKKS.level)           |
| Property   | [CKKS.batch_size](#CKKS.batch_size) |

### **CKKS.encode**
* **Summary**
  * Encode data array at the given level and scaling degree
* **Input**
  * **Data** - **VECTOR**: data to be encoded
  * **Len** - **INT**: data length
  * **Sdegree** - **INT**: encoding scale degree
  * **Level** - **INT**: encoding level
* **Output**
  * **Plain** - **PLAIN**: encoded plaintext

### **CKKS.add**
* **Summary**
  * Performs ciphertext addition with ciphertext or plaintext or a float value
* **Input**
  * **A** - **CIPHER**: first operand to be added
  * **B** - **CIPHER|PLAIN|FLOAT**: second operand to be added
* **Output**
  * **C** - **CIPHER**: result

### **CKKS.sub**
* **Summary**
  * Performs ciphertext subtraction using either ciphertext, plaintext or a float value
* **Input**
  * **A** - **CIPHER**: first operand
  * **B** - **CIPHER|PLAIN|FLOAT**: second operand
* **Output**
  * **C** - **CIPHER**: result

### **CKKS.mul**
* **Summary**
  * Performs ciphertext multiplication using either ciphertext, plaintext or a float value
* **Input**
  * **A** - **CIPHER**: first operand
  * **B** - **CIPHER|PLAIN|FLOAT**: second operand
* **Output**
  * **C** - **CIPHER**: result

### **CKKS.rotate**
* **Summary**
  * Performs ciphertext rotation at specified rotation index
* **Input**
  * **A** - **CIPHER**: ciphertext to be rotated
  * **B** - **INT**: rotation index
* **Output**
  * **C** - **CIPHER**: result, rotated ciphertext
### **CKKS.rescale**
* **Summary**
  * Switches the input ciphertext modulus  from $q_1...q_k$ down to $q_1...q_{k-1}$, and scales the message down accordingly
* **Input**
  * **A** - **CIPHER**: ciphertext to be rescaled
* **Output**
  * **B** - **CIPHER**: result

### **CKKS.upscale**
* **Summary**
  * Increases a ciphertext's scaling factor with given scale bits. C.sf = A.sf * 2^B.
  * Implemented by multiplying with the constant "1.0", which is encoded with specified bits.
* **Input**
  * **A** - **CIPHER**: ciphertext to be upscaled
  * **B** - **INT**: bits
* **Output**
  * **C** - **CIPHER**: result

### **CKKS.modswitch**
* **Summary**
  * Drop the last modular $q_k$, where the orignal modulus are $q_1...q_k$
* **Input**
  * **A** - **CIPHER**: input ciphertext
* **Output**
  * **B** - **CIPHER**: result

### **CKKS.reline**
* **Summary**
  * Performs relinearization operation to convert 3-dimensional ciphertext to 2-dimension
* **Input**
  * **A** - **CIPHER3**: input 3-dimension ciphertext
* **Output**
  * **B** - **CIPHER**: result

### **CKKS.bootstrap**
* **Summary**
  * Performs bootstrap to refersh the computation of a ciphertext, B can be set to adjust the level avaiable after bootstrap
* **Input**
  * **A** - **CIPHER**: input ciphertext
  * **B** - **INT**: level after bootstrap
* **Output**
  * **C** - **CIPHER**: result ciphertext after bootstrap

### **CKKS.scale**
* **Summary**
  * Get the ciphertext's scaling degree
* **Input**
  * **A** - **CIPHER**: input ciphertext
* **Output**
  * **B** - **INT**: scaling degree

### **CKKS.level**
* **Summary**
  * Get the ciphertext's level
* **Input**
  * **A** - **CIPHER**: input ciphertext
* **Output**
  * **B** - **INT**: level

### **CKKS.batch_size**
* **Summary**
  * Get the ciphertext's slot size
* **Input**
  * **A** - **CIPHER**: input ciphertext
* **Output**
  * **B** - **INT**: slot size
  