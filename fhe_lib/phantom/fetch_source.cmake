if(CUSTOM_FETCHCONTENT_DIR)
    set(CUSTOM_PHANTOM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_PHANTOM_NAME})
    file(MAKE_DIRECTORY ${CUSTOM_PHANTOM_DIR})
    set(FETCHCONTENT_SOURCE_DIR_CKKS_INFRA ${CUSTOM_PHANTOM_DIR})
endif()

include(FetchContent)

FetchContent_Declare(
    phantom
    GIT_REPOSITORY  ${DEP_PHANTOM_URL}
    GIT_TAG         ${DEP_PHANTOM_REF}
)

FetchContent_MakeAvailable(phantom)

install(
    DIRECTORY ${phantom_SOURCE_DIR}/include/
    DESTINATION ace/include/lib_phantom
    COMPONENT library
    FILES_MATCHING PATTERN "*.h" PATTERN "*.cuh"
)
