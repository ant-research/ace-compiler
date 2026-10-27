# Test Content

## Preparing a DOCKER environment to Build and Test

Pull the pre-built docker image from Docker Hub (the same image the
`delivery/final-202610` GitHub Actions workflow builds and pushes from
this repo — see `.github/workflows/build-image.yml`):

```
docker pull opencc/ace:final-202610
```

Or build the image locally:

```
docker build . -t opencc/ace:final-202610
```

Start a container once per test item — the `--name` differs per item so
the containers do not collide. CPU test items (§1 / §2 / §3 / §5):

```
docker run -it -d --name test_7.x opencc/ace:final-202610 bash
docker exec -it test_7.x bash
```

GPU test item (§4.1) — pass through the GPU:

```
docker run -it -d --name test_7.4 --gpus '"device=1"' opencc/ace:final-202610 bash
docker exec -it test_7.4 bash
```

The image does NOT pre-build any FHE baseline — it installs the pip
deps and tools only. Run the per-baseline build scripts
(`scripts/build_cnn.py` / `build_cnn_gpu.py` / `build_cnn_fpga.py`)
inside the container before the inference step.

## 1. Testing the Inference Performance of Neural Networks for Fully Homomorphic Encryption

- Setup

    ```
    docker run -it -d --name test_7.1 opencc/ace:final-202610 bash
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
    docker run -it -d --name test_7.2 opencc/ace:final-202610 bash
    docker exec -it test_7.2 bash
    ```

- Inference

    ```
    pytest tests/regression/lenet_x2/test_lenet_x2_smoke.py -v -s
    pytest tests/regression/alexnet_x2/test_alexnet_x2_smoke.py -v -s
    pytest tests/regression/vgg16_x2/test_vgg16_x2_smoke.py -v -s
    pytest tests/regression/resnet_x2/test_resnet20_x2_smoke.py -v -s
    ```

## 3 Testing the Inference Accuracy of Neural Network Models for Fully homomorphic Encryption

- Setup

    ```
    docker run -it -d --name test_7.3 opencc/ace:final-202610 bash
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
    docker run -it -d --name test_7.4 --gpus '"device=1"' opencc/ace:final-202610 bash
    docker exec -it test_7.4 bash
    ```

    H20
    ```
    root@99c62073fab2:/app# nvidia-smi
    Sun Jun 29 18:40:19 2025
    +-----------------------------------------------------------------------------------------+
    | NVIDIA-SMI 550.144.03             Driver Version: 550.144.03     CUDA Version: 12.4     |
    |-----------------------------------------+------------------------+----------------------+
    | GPU  Name                 Persistence-M | Bus-Id          Disp. A | Volatile Uncorr. ECC |
    | Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |
    |                                         |                        |               MIG M. |
    |=========================================+=========================================================+
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
    | GPU  Name                 Persistence-M | Bus-Id          Disp. A | Volatile Uncorr. ECC |
    | Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |
    |                                         |                        |               MIG M. |
    |=========================================+=========================================================+
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

The IIE FPGA accelerator supports running compiler-generated programs and handwritten FHE-MP-CNN programs. Compiler test models include LR, LeNet5, ResNet20, and ResNet32; handwritten implementation test models include ResNet20 and ResNet32.

- Baseline verification

Follow the instructions in `baselines/fpga/README.md` in the delivery repository to complete board configuration and FPGA loading. Run the following commands in the root directory of the `fheacc-software-platform` repository to generate hardware instructions and data for the compiler test models:

```bash
make soft TEST_NAME=lr       RELEASE_MODE=2
make soft TEST_NAME=LeNet5   RELEASE_MODE=2
make soft TEST_NAME=resnet20 RELEASE_MODE=2
make soft TEST_NAME=resnet32 RELEASE_MODE=2
```

The artifacts for each model are located in `soft/clib/tests/tests/<TEST_NAME>/`, where `data/` contains the instructions and HBM image, and `nttw/ntt_intt_merged/` contains the NTT window data. `RELEASE_MODE=2` also runs the CPU Golden model for numerical verification.

Generate the hardware package for the handwritten FHE-MP-CNN test model via the SDK app:

```bash
cd soft/clib/sdk/apps/resnet20
make hardware MODEL=20 SAMPLE=0 RELEASE_MODE=2
# First load and test the generated resnet20/ hardware package, then generate the next model.
make hardware MODEL=32 SAMPLE=0 RELEASE_MODE=2
```

Both models are output to the `resnet20/` hardware package directory under the app directory; the second export overwrites the first. The hardware package name is determined by the app directory name, while the actual network is selected by `MODEL=20` or `MODEL=32`. If only hardware artifacts and cycle estimates are needed, `RELEASE_MODE=1` can be used; this mode does not perform Golden numerical verification.

Use the on-board loader to load the corresponding instruction stream, initial HBM data, and NTT window data. Run the accelerator, read back the output, and compare it with the corresponding Golden result. When performing decryption comparison, if the output is still in Montgomery representation, a de-Montgomery conversion should be performed first; the decryption tool of the SDK hardware package will handle the output format based on what is recorded in the package.

- FPGA performance testing

**The elapsed time includes input data loading, FPGA computation, and result readback.** Compiler-generated code and handwritten implementations use the same timing scope. Save the on-board run logs and timing data to the `testing/` directory of the delivery repository.

The measured results are in the accompanying `report.md`, whose content corresponds to `testing/report.md` in the repository. These data are FPGA measured elapsed times; the run time of `make soft` and the cycle estimates in `clib_profile.md` are different metrics.

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
