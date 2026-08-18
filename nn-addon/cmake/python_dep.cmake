# Find the Pybind11 library
find_package(pybind11 REQUIRED)

if (pybind11_FOUND)
    message(STATUS "pybind11_INCLUDE_DIRS: ${pybind11_INCLUDE_DIRS}")
    message(STATUS "pybind11_LIBRARIES: ${pybind11_LIBRARIES}")
else ()
    message(FATAL_ERROR "pybind11 not found. Please make sure pybind11 is installed.")
endif ()

find_package(Python REQUIRED COMPONENTS Development)
if (Python_FOUND)
    message (STATUS "Python_INCLUDE_DIRS         : ${Python_INCLUDE_DIRS}")
    message (STATUS "Python_LIBRARIES            : ${Python_LIBRARIES}")	
else ()
    message(FATAL_ERROR "Python not found. Please make sure Python is installed.")
endif ()

# set include directories
include_directories(${Python_INCLUDE_DIRS})
include_directories(${pybind11_INCLUDE_DIRS}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)