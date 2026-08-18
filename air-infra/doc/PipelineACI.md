# ACI Pipeline Design and Implementation

**Important**: Markdown file (.md) is the master copy. PDF file (.pdf) is exported from markdown file only for review.

## Revision History

|Version|Author     |Date      |Description|
|-------|-----------|----------|-----------|
|0.1    |linjie.xiao|2023.08.10|Initial version.|
|0.2    |linjie.xiao|2023.11.06|Add Integration & Performance Testing |
|0.3    |linjie.xiao|2024.07.31|Improve CICD Pipeline |


## Introduction
This design document mainly describes the input and output states of each stage of the pipeline and the main Pipeline in the ACI automation CI/CD environment. These include Push/PullRequest, Daily Build, Daily Offline Test, Code Style Check, and Check Build Options.

- [Pipeline Flow chart](./PipelineACI.html)

## CI: Push/PullRequest Pipeline

- Pipeline for Push/PullRequest


```mermaid
flowchart LR
A((PUSH))
B[(Check)]
C[(Report<br>Suite</br>)]
D(Review)
E(Merge to Master)
F[[Check]]
H[(OSS/package<br>Master</br>)]
I[(Report<br>Suite</br>)]
X>Notify Message]

A-.->B
E==>F

F03-.->H
F13-.->H
F23-.->H

F03-.->I
F13-.->I
F23-.->I

I-.->X

subgraph Push branch: Check Coding Conversion/Build/UnitTest
B-->B01[[Build X86_64]]-->B02[[Unittest]]-.->C
B-->B11[[Build AARCH64]]-->B12[[Unittest]]-.->C
B-->B21[[Build ...]]-->B22[[Unittest]]-.->C
end

subgraph Code Review: Comment/Update/Merge
C-.->D01(MergeRequest)-.->D-.->E
D-.->D
end

subgraph master Branch: Check Coding Conversion/Build/UnitTest
F-->F01[[Build X86_64]]-->F02[[Unittest]]-->F03[Update]
F-->F11[[Build AARCH64]]-->F12[[Unittest]]-->F13[Update]
F-->F21[[Build ...]]-->F22[[Unittest]]-->F23[Update]
end
```

- SubPipeline for Package Build & UnitTest

```mermaid
flowchart LR
A((Package))
C[(Report<br>Suite</br>)]

A-.->B00
A-.->B10
A-.->B20
A-.->B30

B01-.->C
B11-.->C
B21-.->C
B31-.->C

subgraph Package: Build/UnitTest
B00[[Build Debug]]-->B01[[Unittest]]
B10[[Build Release]]-->B11[[Unittest]]
B20[[Build Debug Parallel]]-->B21[[Unittest]]
B30[[Build Release Parallel]]-->B31[[Unittest]]
end
```

## CI: Nightly Schedule Pipeline

- Pipeline for Nightly Package Build & Unittest, Image Build

```mermaid
flowchart LR
A((SCHEDULE))
C[(OSS/Package<br>Nightly</br>)]
D[(Register<br>Image:tag</br>)]
E[(Report<br>Suite</br>)]
X>Notify Message]

A-.->B00
A-.->B10
A-.->B20

B02-.->C
B12-.->C
B22-.->C

B03-.->D
B13-.->D
B23-.->D

B04-.->E
B14-.->E
B24-.->E

E-.->X

subgraph Schedule: X86_64 Package Build/UnitTest/Upload/Images
B00[[Check]]-->B01[[Build X86_64]]-->B02[Upload]-->B03[[Image]]-->B04[[Unittest]]
end
subgraph Schedule: AARCH64 Package Build/UnitTest/Upload/Images
B10[[Check]]-->B11[[Build AARCH64]]-->B12[Upload]-->B13[[Image]]-->B14[[Unittest]]
end
subgraph Schedule: Package ... Build/UnitTest/Upload/Images
B20[[Check]]-->B21[[Build ...]]-->B22[Upload]-->B23[[Image]]-->B24[[Unittest]]
end
```


- SubPipeline for Package Build & UnitTest & BenchTest

```mermaid
flowchart LR
A((Package))
C[(Report<br>Suite</br>)]
X>Notify Message]

A-.->B00
A-.->B10
A-.->B20
A-.->B30
A-.->B40

B02-.->C
B12-.->C
B22-.->C
B32-.->C
B42-.->C

C-.->X

subgraph Package: Build/UnitTest/BenchTest
B00[[Build Debug]]-->B01[[UnitTest]]-->B02[[BenchTtest]]
B10[[Build Release]]-->B11[[UnitTest]]-->B12[[BenchTtest]]
B20[[Build Debug Parallel]]-->B21[[UnitTest]]-->B22[[BenchTtest]]
B30[[Build Release Parallel]]-->B31[[UnitTest]]-->B32[[BenchTtest]]
B40[[Build Release Perf]]-->B41[[UnitTest]]-->B42[[BenchTtest]]
end
```

- Pipeline for Build Options Check

```mermaid
flowchart LR
A((SCHEDULE))
B[(Report<br>Suite</br>)]
X>Notify Message]

A -.-> BUILD_UNITTEST & BUILD_BENCH & BUILD_TEST & BUILD_EXAMPLE & INSTALL_APP & ...... -.-> B
B-.->X
```

## CD: Nightly/Weekly Test Pipeline

- Pipeline for Nightly Package Test

```mermaid
flowchart LR
A[(Register<br>Image:tag</br>)]
B(Deploy)
C[(Report<br>Summary</br>)]
X>Notify Message]

A-.->B

B-.->B01
B-.->B11
B-.->B21

B04-.->C
B14-.->C
B24-.->C

C-.->X

subgraph  Image: Debug Package Deploy/Test
B01[[Unittest]] --> B02[[BenchTest]] --> B03[[E2E_Test]] -.->B04[(Report<br>Suite</br>)]
end

subgraph Image: Release Package Deploy/Test
B11[[Unittest]] --> B12[[BenchTest]] --> B13[[E2E_Test]] -.->B14[(Report<br>Suite</br>)]
end

subgraph Image: Parallel Package Deploy/Test
B21[[Unittest]] --> B22[[BenchTest]] --> B23[[E2E_Test]] -.->B24[(Report<br>Suite</br>)]
end
```

- Pipeline for Nightly/Weekly Benchmark Test

```mermaid
flowchart LR
A[(Register<br>Image:tag</br>)]
B(Deploy)
C[(Report<br>Summary</br>)]
X>Notify Message]

A-.->B

B-.->B01
B-.->B11
B-.->B21
B-.->B31

B04-.->C
B14-.->C
B24-.->C
B34-.->C

C-.->X

subgraph Image: Release Package Deploy/Test
B01[[Mark for Suite]] --> B02[[BenchTest]] --> B03[[E2E_Test]] -.->B04[(Report<br>Suite</br>)]
end

subgraph Image: Parallel Package Deploy/Test
B11[[Mark for Suite]] --> B12[[BenchTest]] --> B13[[E2E_Test]] -.->B14[(Report<br>Suite</br>)]
end

subgraph Image: Perf Package Deploy/Test
B21[[Mark for Suite]] --> B22[[BenchTest]] --> B23[[E2E_Test]] -.->B24[(Report<br>Suite</br>)]
end

subgraph Image: ... ... Package Deploy/Test
B31[[Mark for Suite]] --> B32[[BenchTest]] --> B33[[E2E_Test]] -.->B34[(SReport<br>Suite</br>)]
end
```
