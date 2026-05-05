# ACI Pipeline Design and Implementation

**Important**: Markdown file (.md) is the master copy. PDF file (.pdf) is exported from markdown file only for review.

## Revision History

|Version|Author     |Date      |Description|
|-------|-----------|----------|-----------|
|0.1    |linjie.xiao|2023.08.10|Initial version.|
|0.2    |linjie.xiao|2023.11.06|Add Integration & Performance Testing |


## Introduction
This design document mainly describes the input and output states of each stage of the pipeline and the main Pipeline in the ACI automation CI/CD environment. These include Push/PullRequest, Daily Build, Daily Offline Test, Code Style Check, and Check Build Options.

## Push/PullRequest Pipeline

- Total Push/PullRequest Pipeline


```mermaid
flowchart LR
A[[PUSH New Branch]]
C[[MergeRequest]]
D[[Review]]
E[[Merge]]
G[(OSS/master)]
X>Notify to DingTalk]

A-->B00
B01-.->C
C-.->D
D-.->E
E-->F00
F02-.->G
G-.->X

subgraph branch: Check Coding Conversion & Build
B00((Check))==>B01((Build))
end
subgraph master: Check Coding Conversion & Build
F00((Check))==>F01((Build))==>F02((Upload))
end
```

- Push Pipeline

```mermaid
flowchart LR
A[[PUSH New Branch]]
X>Notify to DingTalk]

A-->B00
A-->B10
A-->B20
A-->B30

B01-.->X
B11-.->X
B21-.->X
B31-.->X

subgraph Build Debug On X86_64
B00((Check))==>B01((Build))
end
subgraph Build Release On X86_64
B10((Check))==>B11((Build))
end
subgraph Build Debug On AARCH64
B20((Check))==>B21((Build))
end
subgraph Build Release On AARCH64
B30((Check))==>B31((Build))
end
```

- Merge to Maser Pipeline

```mermaid
flowchart LR
A[[Merge to Master]]
C[(OSS/master)]
X>Notify to DingTalk]

A-->B00
A-->B10
A-->B20
A-->B30

B02-.->C
B12-.->C
B22-.->C
B32-.->C

C-.->X

subgraph Build Debug On X86_64
B00((Check))==>B01((Build))==>B02((Upload))
end
subgraph Build Release On X86_64
B10((Check))==>B11((Build))==>B12((Upload))
end
subgraph Build Debug On AARCH64
B20((Check))==>B21((Build))==>B22((Upload))
end
subgraph Build Release On AARCH64
B30((Check))==>B31((Build))==>B32((Upload))
end
```

## Daily Build Pipeline

- Build Daily Package Pipeline

```mermaid
flowchart LR
A[[SCHEDULE]]
C[(OSS/daily)]
D[(Images)]
X>Notify to DingTalk]

A-->B00
A-->B10
A-->B20
A-->B30

B02-.->C
B12-.->C
B22-.->C
B32-.->C

B03-.->D
B13-.->D
B23-.->D
B33-.->D

C-.->X
D-.->X

subgraph Build Debug On X86_64
B00((Check))==>B01((Build))==>B02((Upload))==>B03((Image))
end
subgraph Build Release On X86_64
B10((Check))==>B11((Build))==>B12((Upload))==>B13((Image))
end
subgraph Build Debug On AARCH64
B20((Check))==>B21((Build))==>B22((Upload))==>B23((Image))
end
subgraph Build Release On AARCH64
B30((Check))==>B31((Build))==>B32((Upload))==>B33((Image))
end
```

- Check Build Options Pipeline

```mermaid
flowchart LR
A[[SCHEDULE]]
X>Notify to DingTalk]

A-.->A0
A3-.->X

subgraph Check Build Options
A0((Clone)) --> BUILD_UNITTEST & BUILD_BENCH & BUILD_TEST & BUILD_EXAMPLE & INSTALL_APP --> A3((Report))
end
```

- Check Regex with Coding Conversion

```mermaid
flowchart LR
A[[Update regex]]
B[(OSS/regex)]
C[(OSS/libs)]
D[(Images)]
A-.->B
B-.->A0
A1-.->C
A2-.->D
subgraph Build regex lib
A0((Build))==>A1((regex lib))==>A2((image))
end
```

## CTI Daily Test Pipeline

- CTI Daily Integration Testing

```mermaid
flowchart LR
A[[Monitor]]
B[(Images)]
C[(HTML Report)]
X>Notify to DingTalk]

A-.->B00
B-->B00

B01-.->C
C-.->X

subgraph CTI Daily Integration Testing
B00((Deploy))--> Debug_UNITTEST & Debug_EXAMPLE & Debug_TEST & Debug_Binary & Debug_Verify -->B01((json))
end
```

- CTI Daily Benchmark Testing

```mermaid
flowchart LR
A[[Monitor]]
B[(Images)]
C[(HTML Report)]
X>Notify to DingTalk]

A-.->B00
B-->B00

B01-.->C
C-.->X

subgraph CTI Daily Benchmark Testing
B00((Deploy))--> Release_Benchmark & Release_Comparison & Release_Performance -->B01((json))
end
```



