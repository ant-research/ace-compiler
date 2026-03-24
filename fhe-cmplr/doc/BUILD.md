# BUILD **fhe-cmplr**

## Clone SourceCode

- Clone repos by script

After entering Docker, Run the **clone_avhc.sh** script to automatically clone AVHC repos。
```
clone_avhc.sh
```

- or Clone repos manually

fhe-cmplr requires three repos **air-infra**, **nn-addon**, **fhe-cmplr** to be compiled in the same directory, please Clone these repos to the same directory.

```
mkdir workarea && cd workarea
git clone https://code.alipay.com/air-infra/air-infra.git
git clone https://code.alipay.com/air-infra/nn-addon.git
git clone https://code.alipay.com/fhe-cmplr/fhe-cmplr.git
```

## Build Options
- Default Build Options Setting

    Global options
    ```
    -DCMAKE_BUILD_TYPE=Debug|Release        # default Debug
    -DBUILD_STATIC=ON|OFF                   # default ON
    -DBUILD_SHARED=ON|OFF                   # default OFF       TODO: implement later
    -DBUILD_UNITTEST=ON|OFF                 # default ON
    -DBUILD_BENCH=ON|OFF                    # default ON
    -DBUILD_DOC=ON|OFF                      # default OFF
    ```
    Current repo options
    ```
    -DFHE_CODE_CHECK=ON|OFF                 # default ON        NOTE: if want close code check, Manually set to OFF
    -DFHE_WITH_SRC="air-infra;nn-addon"     # default OFF
    -DFHE_BUILD_TEST=ON|OFF                 # default ON
    -DFHE_BUILD_EXAMPLE=ON|OFF              # default ON
    -DFHE_INSTALL_APP=ON|OFF                # default ON
    -DBUILD_WITH_OPENMP=ON|OFF              # default OFF
    ```

## Build **fhe-cmplr**

- Example: Build fhe-cmplr with "air-infra;nn-addon" source

    [Clone SourceCode](SETUP.md)

    ```
    cmake -S fhe-cmplr -B build -DFHE_WITH_SRC="air-infra;nn-addon"
    cmake --build build -- -j"N"
    ```

- Example: Build fhe-cmplr with "air-infra,nn-addon" libs

    [Install libs with air-infra & nn-addon](SETUP.md)

    ```
    cmake -S fhe-cmplr -B build
    cmake --build build -- -j"N"
    ```

- Example: Build fhe-cmplr with openmp
 
    ```
    cmake -S fhe-cmplr -B build -DFHE_WITH_SRC="air-infra;nn-addon" -DBUILD_WITH_OPENMP=ON
    cmake --build build -- -j"N"
    ```

- Example: Build air-infra with Debug

    ```
    cmake -S fhe-cmplr -B debug -DCMAKE_BUILD_TYPE=Debug
    cmake --build debug -- -j"N"
    ```

- Example: Build air-infra without code style check

    ```
    cmake -S fhe-cmplr -B build -DFHE_CODE_CHECK=OFF
    cmake --build build -- -j"N"
    ```

- Install

    ```
    cmake --install build
    ```

## **fhe-cmplr** Packages

Daily builds of dependence libraries on ACI, include debug and release versions.
The storage path is: oss://antsys-fhe/daily/***"DATE"***/***fhe-cmplr_deb/rel.tar.gz***
Example: Get the dependency library and install, such as:

```
ossutil64 cp oss://antsys-fhe/daily/20241114/fhe-cmplr_deb.tar.gz .
tar xf air-infra_deb.tar.gz -C /usr/local
```