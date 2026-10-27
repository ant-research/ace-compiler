# Phantom FHE with Bootstrapping

This is a fork of the [Phantom FHE](https://github.com/encryptorion-lab/phantom-fhe) repository, with added bootstrapping functionality and additional examples.

## Building the Project

To build this project:

1. Clone this repository to your environment with CUDA.

2. Create and navigate to the build directory:
cmake -S . -B build

3. Build the project:
cmake --build build -j


4. Run tests:
cmake --build build -t test


## Running the CNN Example

You can run the CNN example from FHE-MP-CNN:
./build/bin/cnn 20 10 0 0


Note: If you encounter any issues, please check if the required directories exist.

## Issues and Contributions

If you encounter any problems or have suggestions for improvements, please open an issue or submit a pull request.