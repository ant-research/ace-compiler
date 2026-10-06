## Dependencies
The GMP library and the NTL library is needed for the Remez algorithm.
OpenMP library is needed for the multi-threaded execution of cnn.
All programs for building the SEAL library is needed.
This source code has been developed and checked in Ubuntu-20.04.
384GB RAM is required to test one image. (not exact hard limit but encouraged)
512GB RAM is required to test 50 images simultaneously with multi-threading. (not exact hard limit but encouraged)

## Regarding SEAL library
We use Microsoft SEAL library version 3.6.6 for RNS-CKKS homomorphic encryption scheme. Since the original SEAL library version 3.6.6 is not bootstrapping-friendly, we modified the SEAL library so that the bootstrapping operation of the RNS-CKKS scheme can be implemented. You should build and install the modified SEAL library in "cnn_ckks/cpu-ckks/single-key/seal-modified-3.6.6" if you want to build our homomorphic ResNet CNN source code. Specifically, you can build and install the modified SEAL library by the following commands.

```PowerShell
cd cnn_ckks/cpu-ckks/single-key/seal-modified-3.6.6
cmake -S . -B build
cmake --build build
cmake --install build
```

## Building cnn_ckks 
The executable file can be built by the following commands.

```PowerShell
cd cnn_ckks
cmake -S . -B build
cd build
make
```

The build outputs including executable is stored in "cnn_ckks/build" directory, and the executable file is named as "cnn".

## Executing the executable file
There are some additional parameters when executing the "cnn" file, and the form of the command should be as follows.

```PowerShell
cd cnn_ckks/build
./cnn (LAYER NUMBER) (DATASET NUMBER) (START IMAGE) (END IMAGE)
```

(LAYTER NUMBER) : the number of layers in cnn
(DATASET NUMBER) : the type of dataset (10: CIFAR-10, 100: CIFAR-100)
(START IMAGE) : the label number of the first image you want to infer
(END IMAGE) : the label number of the last image you want to infer

For example, if you want to perform ResNet-110 for images in CIFAR-10 test dataset with label number 6, 7, 8, 9, and 10, you may want to execute the "cnn" file as follows.

```PowerShell
cd cnn_ckks/build
./cnn 110 10 6 10 
```

If you want to perform ResNet-32 for only an image in CIFAR-100 with label number 4, you may want to execute the "cnn" file as follows.

```PowerShell
cd cnn_ckks/build
./cnn 32 100 4 4 
```

Supported layers : CIFAR-10 - 20, 32, 44, 56, 110 / CIFAR-100 - 32
Supported image label numbers : 0 ~ 9999

## Checking results
Text files for various intermediate and final results are generated in the result directory in the root directory of our supplementary file.
The following text files are generated.

resnet(LAYER NUMBER)_cifar(DATASET NUMBER)_image(IMAGE NUMBER).txt: This file includes information about the running time, the remaining level, and the scaling factor for each procedure. Also, it partly shows the decrypted values when each layer is terminated and shows the resultant decrypted values. Finally, it shows the inference result with the correct image label and total running time. This type of file is generated for each input image.

resnet(LAYER NUMBER)_cifar(DATASET NUMBER)_label_(START IMAGE)_(END IMAGE): This file includes information about the inference results with the correct image label for all images.
