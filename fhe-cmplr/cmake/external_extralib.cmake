# check if Protobuf is installed
# https://cmake.org/cmake/help/latest/module/FindProtobuf.html
function(find_protobuf_lib MESSAGE)
  find_package(Protobuf REQUIRED)

  if (Protobuf_FOUND)
    include_directories(${Protobuf_INCLUDE_DIRS})
    if (APPLE)
      find_library(AbslLogMessageLib absl_log_internal_message)
      find_library(AbslLogCheckOpLib absl_log_internal_check_op)
      set (EXTRA_LIBS ${EXTRA_LIBS} ${Protobuf_LIBRARIES} ${AbslLogMessageLib} ${AbslLogCheckOpLib})
    else ()
      set (EXTRA_LIBS ${EXTRA_LIBS} ${Protobuf_LIBRARIES})
    endif ()
  else ()
    message (FATAL_ERROR "** ERROR ** Protobuf is not installed")
  endif ()

  set (EXTRA_LIBS ${EXTRA_LIBS} PARENT_SCOPE)
endfunction ()

# check if gmp is installed
function(find_gmp_lib MESSAGE)
  find_library (GMP_LIBRARY NAMES gmp libgmp)

  if (GMP_LIBRARY)
    set (MATH_LIBS ${MATH_LIBS} ${GMP_LIBRARY} PARENT_SCOPE)
  else ()
    message (FATAL_ERROR "** ERROR ** libgmp is not installed")
  endif ()

endfunction ()

# check if math is installed
function(find_math_lib MESSAGE)
  find_library (M_LIBRARY NAMES m)

  if (M_LIBRARY)
    set (MATH_LIBS ${MATH_LIBS} ${M_LIBRARY} PARENT_SCOPE)
  else ()
    message (FATAL_ERROR "** ERROR ** math is not installed")
  endif ()

endfunction ()

if (BUILD_WITH_OPENMP)
  find_package(OpenMP)
	# OpenMP_CXX_FOUND was added in cmake 3.9.x so we are also checking
  # the OpenMP_FOUND flag
	if (OpenMP_CXX_FOUND OR OpenMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
	else()
		message(SEND_ERROR "** ERROR ** OpenMP is not installed")
	endif()

	if (OpenMP_C_FOUND OR OpenMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	endif()
else()
	# Disable unknown #pragma omp warning
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas")
endif()

find_protobuf_lib("find lib : Protobuf")
find_gmp_lib("find lib : libgmp")
find_math_lib("find lib : libm")