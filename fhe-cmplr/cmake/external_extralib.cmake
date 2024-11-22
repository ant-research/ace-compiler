# https://cmake.org/cmake/help/latest/module/FindProtobuf.html

# check if Protobuf is installed
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
  message (FATAL_ERROR "Protobuf need to be installed")
endif ()

# check if gmp is installed
find_library (GMP_LIBRARY NAMES gmp libgmp)
if (GMP_LIBRARY)
  set (MATH_LIBS ${MATH_LIBS} ${GMP_LIBRARY})
else ()
  message (FATAL_ERROR "libgmp need to be installed")
endif ()

# check if m is installed
find_library (M_LIBRARY m)
if (M_LIBRARY)
  set (MATH_LIBS ${MATH_LIBS} ${M_LIBRARY})
else ()
  message (FATAL_ERROR "libm need to be installed")
endif ()