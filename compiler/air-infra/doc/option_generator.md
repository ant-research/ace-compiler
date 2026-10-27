# AIR option

**Important**: Markdown file (.md) is the master copy. PDF file (.pdf) is exported from markdown file only for review.

## Introduction

This document describes the configuration and usage of AIR options. Adding and configuring option information is done through yaml files.
AIR provides a python script option_generator.py to parse this yaml file and generate the required C++ option file.


## How to write option detail yaml files

- Refer to nn-addon/vector/config/option.yml file.

## How to using python script option_generator.py

```
python3 option_generator.py -ip option.yml
```

This will generate option_config.h and option_config.inc files in current path.

## How to integrate into cmake
We have defined gen_option function in air-infra/cmake/funcion_common.cmake file,
you can modify CMakeLists.txt(in your phase) file to invoke gen_option to generate option for your phase.

Generate vector phase option example: 
- add below information to nn-addon/vector/CMakeLists.txt file:

```
# generate vector options
set(VOPTION_OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/nn/vector")
set(VOPTION_INPUT_DIR "vector/config")
gen_option(${PYTHON_EXECUTABLE} voption ${VOPTION_OUTPUT_DIR} ${VOPTION_INPUT_DIR})
add_dependencies(nn_depend voption)

```

## How to integrate into your phase
You can extend the functionality by inheriting the class in option_config.h file, like nn-addon/include/nn/vector/config.h file.
You need to add a cxx file in your phase to include the automatically generated inc file to avoid link errors, like nn-addon/vector/src/option.cxx file.
