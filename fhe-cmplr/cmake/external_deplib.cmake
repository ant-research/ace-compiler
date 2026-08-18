# check if air-infra is installed
function(find_air_lib MESSAGE)
  find_library (AIRUTIL NAMES AIRutil)
  find_library (AIRBASE NAMES AIRbase)
  find_library (AIRCORE NAMES AIRcore)
  find_library (AIRCG NAMES AIRcg)
  find_library (AIROPT NAMES AIRopt)
  find_library (AIRDRIVER NAMES AIRdriver)

  if (AIRUTIL AND AIRBASE AND AIRCORE AND AIRCG AND AIROPT AND AIRDRIVER)
    set (FHE_DEP_AIR_LIBS ${FHE_DEP_AIR_LIBS} ${AIRDRIVER} ${AIROPT} ${AIRCG} ${AIRCORE} ${AIRBASE} ${AIRUTIL})
  else ()
    message (FATAL_ERROR "** ERROR ** air-infra is not installed")
  endif ()

  set (FHE_DEP_AIR_LIBS ${FHE_DEP_AIR_LIBS} PARENT_SCOPE)
endfunction ()

# check if nn-addon is installed
function(find_nn_lib MESSAGE)
  find_library (NNUTIL NAMES NNutil)
  find_library (NNCORE NAMES NNcore)
  find_library (NNONNX NAMES NNonnx)
  find_library (NNOPT NAMES NNopt)
  find_library (NNONNX2AIR NAMES NNonnx2air)
  find_library (NNVECTOR NAMES NNvector)
  find_library (NNDRIVER NAMES NNdriver)

  if (NNUTIL AND NNCORE AND NNONNX AND NNOPT AND NNONNX2AIR AND NNVECTOR AND NNDRIVER)
    set (FHE_DEP_NN_LIBS ${FHE_DEP_NN_LIBS} ${NNDRIVER} ${NNVECTOR} ${NNONNX2AIR} ${NNOPT} ${NNONNX} ${NNCORE} ${NNUTIL})
  else ()
    message (FATAL_ERROR "** ERROR ** nn-addon is not installed")
  endif ()

  set (FHE_DEP_NN_LIBS ${FHE_DEP_NN_LIBS} PARENT_SCOPE)
endfunction ()

# check option : -DFHE_WITH_SRC="***"
function(build_with_src MESSAGE)
  string (FIND "${FHE_WITH_SRC}" "air-infra" CONF_AIR_INFRA)
  string (FIND "${FHE_WITH_SRC}" "nn-addon" CONF_NN_ADDON)

  if (${CONF_AIR_INFRA} GREATER "-1")
    set (NN_WITH_SRC "air-infra" CACHE BOOL "Enable build air-infra with src." FORCE)
    include_directories (${CMAKE_SOURCE_DIR}/../air-infra/include)
    include_directories (${CMAKE_BINARY_DIR}/include)
    set (FHE_DEP_AIR_LIBS ${FHE_DEP_AIR_LIBS} AIRdriver AIRopt AIRcg AIRcore AIRbase AIRutil)
  else ()
    find_air_lib("find lib : air-infra")
  endif ()

  if (${CONF_NN_ADDON} GREATER "-1")
    set (NN_ADDON_PROJECTS ${CMAKE_SOURCE_DIR}/../nn-addon)
    if (IS_DIRECTORY ${NN_ADDON_PROJECTS})
      add_subdirectory (${NN_ADDON_PROJECTS} nn-addon)
      include_directories (${NN_ADDON_PROJECTS}/include)
      set (FHE_DEP_NN_LIBS ${FHE_DEP_NN_LIBS} NNdriver NNvector NNonnx2air NNopt NNonnx NNcore NNutil)
    else ()
        message (FATAL_ERROR "** ERROR ** ${NN_ADDON_PROJECTS} can't found")
    endif ()
  endif ()

  set (FHE_DEP_LIBS ${FHE_DEP_NN_LIBS} ${FHE_DEP_AIR_LIBS} PARENT_SCOPE)
endfunction ()

# option : -DFHE_WITH_SRC
if (FHE_WITH_SRC)
  build_with_src("option : -DFHE_WITH_SRC=***")
else ()
  find_air_lib("find lib : air-infra")
  find_nn_lib("find lib : nn-addon")
  set (FHE_DEP_LIBS ${FHE_DEP_NN_LIBS} ${FHE_DEP_AIR_LIBS})
endif ()
