# Test Content

## Preparing a DOCKER environment to Build and Test 

- Pull the pre-built docker image from Docker Hub

    Run on CPU for testing 7.1, 7.2, 7.3
    Run on Accelerator for testing 7.4

    ```
    docker pull opencc/ace:20250524
    ```

- If you want to Build Docker Image locally, you can execute the following command

    ```
    docker build . -t opencc/ace:20250524
    ```

## 1. Testing the Inference Performance of Neural Networks for Fully Homomorphic Encryption

- Setup

    ```
    docker pull opencc/ace:20250524
    docker run -it -d --name test_7.1 opencc/ace:20250524 bash
    docker exec -it test_7.1 bash
    ```

- Build

    ```
    python3 /app/scripts/build_cnn.py   # build the cpu baseline (FHE-MP-CNN, SEAL 3.6.6) + install modified SEAL to /usr/local
    /app/scripts/build_cmplr.sh         # build the ace-compiler (CPU backend, ONNX -> FHE executable)
    ```

- Inference

    ```
    nohup python3 /app/scripts/perf.py -e > /dev/null 2>&1 &
    nohup python3 /app/scripts/cgo25_ae.py -a > /dev/null 2>&1 &
    ```

## 2 Generalization Ability Test of Fully Homomorphic Encryption Compiler

- Setup

    ```
    docker pull opencc/ace:20250524
    docker run -it -d --name test_7.2 opencc/ace:20250524 bash
    docker exec -it test_7.2 bash
    ```

- Build

    ```
    /app/scripts/build_cmplr_omp.sh      # build ace-compiler (OpenMP backend, supports parallel multi-image inference)
    ```

- Inference

    ```
    nohup python3 /app/scripts/resbm.py > /dev/null 2>&1 &
    ```

## 3 Testing the Inference Accuracy of Neural Network Models for Fully homomorphic Encryption

- Setup

    ```
    docker pull opencc/ace:20250524
    docker run -it -d --name test_7.3 opencc/ace:20250524 bash
    docker exec -it test_7.3 bash
    ```

- Build

    ```
    /app/scripts/build_cmplr_omp.sh     # build ace-compiler (OpenMP backend, supports parallel multi-image inference)
    ```

- Inference

    ```
    python3 /app/scripts/resbm_expert.py -n 100
    nohup python3 /app/scripts/resbm.py -n 100 > /dev/null 2>&1 &
    ```

## 4. Performance Testing of Fully homomorphic Compilation Systems for Hardware Accelerators

### 4.1 GPU accelerator (baselines/gpu)

- Setup

    ```
    docker pull opencc/ace:20250524
    docker run -it -d --name test_7.4 --gpus '"device=1"' opencc/ace:20250524 bash
    docker exec -it test_7.4 bash
    ```

    H20
    ```
    root@99c62073fab2:/app# nvidia-smi 
    Sun Jun 29 18:40:19 2025       
    +-----------------------------------------------------------------------------------------+
    | NVIDIA-SMI 550.144.03             Driver Version: 550.144.03     CUDA Version: 12.4     |
    |-----------------------------------------+------------------------+----------------------+
    | GPU  Name                 Persistence-M | Bus-Id          Disp.A | Volatile Uncorr. ECC |
    | Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |
    |                                         |                        |               MIG M. |
    |=========================================+========================+======================|
    |   0  NVIDIA H20                     Off |   00000000:26:00.0 Off |                    0 |
    | N/A   40C    P0             74W /  500W |       4MiB /  97871MiB |      0%      Default |
    |                                         |                        |             Disabled |
    +-----------------------------------------+------------------------+----------------------+
                                                                                            
    +-----------------------------------------------------------------------------------------+
    | Processes:                                                                              |
    |  GPU   GI   CI        PID   Type   Process name                              GPU Memory |
    |        ID   ID                                                               Usage      |
    |=========================================================================================|
    |  No running processes found                                                             |
    +-----------------------------------------------------------------------------------------+
    ```

    A100:
    ```
    root@7a4253dbaa94:/app# nvidia-smi
    Tue Jul  1 12:19:42 2025
    +-----------------------------------------------------------------------------------------+
    | NVIDIA-SMI 570.144                Driver Version: 570.144        CUDA Version: 12.8     |
    |-----------------------------------------+------------------------+----------------------+
    | GPU  Name                 Persistence-M | Bus-Id          Disp.A | Volatile Uncorr. ECC |
    | Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |
    |                                         |                        |               MIG M. |
    |=========================================+========================+======================|
    |   0  NVIDIA A100 80GB PCIe          Off |   00000000:36:00.0 Off |                    0 |
    | N/A   44C    P0             45W /  300W |       0MiB /  81920MiB |      0%      Default |
    |                                         |                        |             Disabled |
    +-----------------------------------------+------------------------+----------------------+
                                                                                            
    +-----------------------------------------------------------------------------------------+
    | Processes:                                                                              |
    |  GPU   GI   CI              PID   Type   Process name                        GPU Memory |
    |        ID   ID                                                               Usage      |
    |=========================================================================================|
    |  No running processes found                                                             |
    +-----------------------------------------------------------------------------------------+
    ```

- Build

    ```
    python3 /app/scripts/build_cnn_gpu.py   # build the gpu baseline (phantom-fhe, CUDA RNS-CKKS + bootstrapping)
    /app/scripts/build_cmplr_gpu.sh         # build the ace-compiler (GPU/CUDA backend, ONNX -> FHE executable)
    ```

- Inference

    ```
    nohup python3 /app/scripts/perf_gpu.py -a > /dev/null 2>&1 &
    ```

### 4.2 FPGA accelerator (baselines/fpga, interface adaptation / CPU verification path)

`baselines/fpga` is a copy of `baselines/cpu` with the underlying SEAL `Evaluator` calls replaced by a `fhe::*` interface layer (aligned to `rt_iie.h`'s high-level wrappers), intended for later FPGA-accelerator hookup. Currently it runs on the CPU verification path — `fhe::*` forwards to SEAL.

- Build

    ```
    python3 /app/scripts/build_cnn_fpga.py   # build the fpga baseline (fhe_ops-adapted cnn_ckks); needs modified SEAL 3.6.6 installed at /usr/local (from build_cnn.py in §1)
    ```

- Inference (ResNet-20 single image)

    ```
    cd /app/baselines/fpga/cnn_ckks/build_fpga
    ./cnn 20 10 0 0
    ```

    `fhe::hadd_inplace` / `hsub` / `pmult` / `rescale` / `rotate` / `relin` / `modswitch` etc. forward to the SEAL Evaluator; the inferred result matches `baselines/cpu`. A single ResNet-20 image takes ~25-40 min (CPU SEAL bootstrap).

## 5. Runtime library unit tests (7.5)

Runtime-library unit tests (examples built by the ace-compiler's `RTLIB_BUILD_EXAMPLE`): polynomial computation per backend. 

```
# CPU — build rtlib examples standalone into release_rtlib/ (does not touch the main ace-compiler release/)
/app/scripts/build_rtlib.sh Release
python3 /app/scripts/run_rtlib.py -e /app/ace-compiler/release_rtlib/example

# GPU
python3 /app/scripts/run_rtlib.py -e /app/ace-compiler/release_gpu/rtlib/build/example

# FPGA
python3 /app/scripts/run_rtlib.py -e /app/ace-compiler/release_fpga/rtlib/build/example
```

## 6. Generate testing report

After any/all test items run, use `gen_report.py` to extract data from the `.log` files and generate `data 7.X.csv` + `figures_7.X.png` + `report.md` (under `/app/testing/`). The script identifies which test item a log belongs to by its filename prefix (`7.1_` / `7.2_` / `7.3_` / `7.4_` / `7.5_`, written by each test script).

- Single item:

    ```
    python3 /app/scripts/gen_report.py -L /app -o /app/testing -t 7.4
    ```

- All five items:

    ```
    python3 /app/scripts/gen_report.py -L /app -o /app/testing
    ```
