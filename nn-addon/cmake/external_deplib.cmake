# check if air-infra is installed
function(find_air_lib MESSAGE)
  find_library (AIRUTIL NAMES AIRutil)
  find_library (AIRBASE NAMES AIRbase)
  find_library (AIRCORE NAMES AIRcore)
  find_library (AIRCG NAMES AIRcg)
  find_library (AIROPT NAMES AIRopt)
  find_library (AIRDRIVER NAMES AIRdriver)

  if (AIRUTIL AND AIRBASE AND AIRCORE AND AIRCG AND AIROPT AND AIRDRIVER)
    set (NN_DEP_LIBS ${NN_DEP_LIBS} ${AIRDRIVER} ${AIROPT} ${AIRCG} ${AIRCORE} ${AIRBASE} ${AIRUTIL})
  else ()
    message (FATAL_ERROR "** ERROR ** air-infra is not installed")
  endif ()

  set (NN_DEP_LIBS ${NN_DEP_LIBS} PARENT_SCOPE)
endfunction ()

# check option : -DNN_WITH_SRC="***"
function(build_with_src MESSAGE)
  string (FIND "${NN_WITH_SRC}" "air-infra" CONF_AIR_INFRA)

  if (${CONF_AIR_INFRA} GREATER "-1")
    set (AIR_INFRA_PROJECTS ${CMAKE_SOURCE_DIR}/../air-infra)
    if (IS_DIRECTORY ${AIR_INFRA_PROJECTS})
      add_subdirectory (${AIR_INFRA_PROJECTS} air-infra)
      include_directories (${AIR_INFRA_PROJECTS}/include)
      include_directories (${CMAKE_BINARY_DIR}/include)
      set (NN_DEP_LIBS ${NN_DEP_LIBS} AIRdriver AIRopt AIRcg AIRcore AIRbase AIRutil)
    else ()
      message (FATAL_ERROR "** ERROR ** ${AIR_INFRA_PROJECTS} can't found")
    endif ()
  endif ()

  set (NN_DEP_LIBS ${NN_DEP_LIBS} PARENT_SCOPE)
endfunction ()

# option : -DNN_WITH_SRC
if (NN_WITH_SRC)
  build_with_src("option : -DNN_WITH_SRC=***")
else ()
  find_air_lib("find lib : air-infra")
endif ()