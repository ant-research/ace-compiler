# BUILD Introduce

## Build Options
- Default Build Options Setting

Global options
```
-DCMAKE_BUILD_TYPE=Debug|Release    # default Release
-DBUILD_STATIC=ON|OFF               # default ON
-DBUILD_SHARED=ON|OFF               # default OFF
-DBUILD_UNITTEST=ON|OFF             # default ON
-DBUILD_BENCH=ON|OFF                # default ON
-DBUILD_DOC=ON|OFF                  # default OFF
```

Current repo options
```
-DAIR_CODE_CHECK=ON|OFF         # default ON    NOTE: if want close code check, Manually set to OFF
-DAIR_BUILD_TEST=ON|OFF         # default ON
-DAIR_BUILD_EXAMPLE=ON|OFF      # default ON
-DAIR_INSTALL_APP=ON|OFF        # default ON
```

## Build

- Example: Build air-infra with Debug

```
cmake -S air-infra -B debug -DCMAKE_BUILD_TYPE=Debug
cmake --build debug -- -j"N"
```

- Example: Build air-infra without code style check

```
cmake -S air-infra -B build -DAIR_CODE_CHECK=OFF
cmake --build build -- -j"N"
```

- Install

```
cmake --install build
```