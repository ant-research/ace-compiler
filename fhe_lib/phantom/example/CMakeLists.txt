# 单元测试列表
set(UNIT_TESTS
    add
    # test_utils
    # test_memory
)

find_library(NTL_LIBRARY ntl)
find_library(GMP_LIBRARY gmp)
find_library(GMPXX_LIBRARY gmpxx)

foreach(test_name ${UNIT_TESTS})
    add_executable(${test_name} ${test_name}.cu)
    
    target_link_libraries(${test_name} 
        PRIVATE 
            FHErt_phantom  ntl gmpxx gmp FHErt_common
            $<$<BOOL:${ENABLE_CUDA}>:CUDA::cudart>
    )
    
  target_include_directories(add
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    PRIVATE
      ${phantom_SOURCE_DIR}/include
      $<TARGET_PROPERTY:compiler_shared,INTERFACE_INCLUDE_DIRECTORIES>
  )

    # 设置 CUDA 属性（如果需要）
    # if(ENABLE_CUDA AND test_name STREQUAL "test_kernel")
    #     set_property(TARGET ${test_name} PROPERTY CUDA_ARCHITECTURES 75 80 86)
    # endif()
    
    add_test(NAME unit_${test_name} COMMAND ${test_name})


endforeach()

