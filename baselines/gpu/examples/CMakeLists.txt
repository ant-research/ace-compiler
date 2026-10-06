# 查找所有源文件
file(GLOB EXAMPLE_SOURCES "*.cu" "*.cpp")

# 为每个源文件创建一个可执行文件
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
    target_link_libraries(${EXAMPLE_NAME} PRIVATE phantom  ${NTL_LIBRARY} ${GMP_LIBRARY} ${GMPXX_LIBRARY})
    
    # 设置CUDA特定的编译选项
    set_target_properties(${EXAMPLE_NAME} PROPERTIES 
        CUDA_SEPARABLE_COMPILATION ON
    )
    target_compile_options(${EXAMPLE_NAME} PRIVATE 
        "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CUDA>>:-G;-src-in-ptx>"
    )
endforeach()