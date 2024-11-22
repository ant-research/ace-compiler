# BUILD Introduce

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
    -DFHE_WITH_SRC="air-infra;nn-addon"     #
    -DFHE_BUILD_TEST=ON|OFF                 # default ON
    -DFHE_BUILD_EXAMPLE=ON|OFF              # default ON
    -DFHE_INSTALL_APP=ON|OFF                # default ON
    -DBUILD_WITH_OPENMP=ON|OFF              # default OFF
    ```

## Build fhe-cmplr

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