# CodeGen Design and Implementation

**Important**: Markdown file (.md) is the master copy. PDF file (.pdf) is exported from markdown file only for review.

## Revision History

|Version|Author     |Date      |Description|
|-------|-----------|----------|-----------|
|0.1    |linjie.xiao|2024.08.29|Initial version.|


## Introduction
This design document mainly describes the process involves the pre-trained model undergoing 5 layers of IR lowering to generate polyIR, which is then converted to RISC-V assembly instructions through CGIR. Finally, the assembler and linker are used to produce a RISC-V executable program.

- [Pipeline Flow chart](./CodeGen.html)

## CI: Push/PullRequest Pipeline

```mermaid
flowchart LR
A[(Model<br>preTrain</br>)]
C[(Object<br>File</br>)]
D[(Object<br>Files</br>)]
E(Exectue<br>RISC-V</br>)
style E fill:#cfc,stroke:#333,stroke-width:2px,color:#000;

A-.->B00
D01-.->C
B03-.->D01

D-.->D11
D11-.->E

subgraph ANT-ACE
B00[(IR<br>Lowering</br>)]-.->B01[(IR<br>Poly</br>)] --> B02(IR<br>Exend RISC-V</br>) --> B03(ASM<br>Extend RISC-V</br>)
end

subgraph Binary Utility
D01(Assembler<br>RISC-V</br>) & D11(Linker<br>RISC-V</br>)
end
```
