cmake_minimum_required (VERSION 3.5)
project (od_system)
set(CMAKE BUILD TYPE Debug)
#set(CMAKE_CURRENT_SOURCE_DIR "C:/Users/peili/OneDrive/Desktop/obdyn/od_solver/build")

include_directories(
  element/inc
  system/inc
  utils/inc
)

set(OD_SRC
  system/od_equset.cxx
  system/od_measure.cxx
  system/od_system.cxx
  system/od_gstiff.cxx
  system/od_parJ_parqi.cxx
  system/od_systemtrack2d.cxx
  system/od_loop.cxx
  system/od_subsystem.cxx
  element/expr.cxx
  element/od_constraint.cxx
  element/od_marker.cxx
  element/od_body.cxx
  element/od_force.cxx
  utils/mat33.cxx
  utils/od_linsolv.cxx
  utils/od_topo.cxx
)
add_library(od_system SHARED ${OD_SRC} )
#install(TARGETS od_system DESTINATION /home/psong/obdyn/lib)
#target_link_libraries(od_system -lstdc++)
if (UNIX)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++")
endif (UNIX)




find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
#SET(CMAKE_SWIG_FLAGS "")
#set(CMAKE_GENERATOR_PLATFORM, x64)
if (PY3)
find_package(PythonInterp 3.5 REQUIRED)
find_package(PythonLibs 3.5 REQUIRED)
else()
find_package(PythonInterp 2.7 REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)
endif()

include_directories(${PYTHON_INCLUDE_DIRS})
#set(CMAKE_SOURCE_DIR, "/home/psong/obdyn/obdyn/src")
link_directories("/home/psong/obdyn/build")

#if (UNIX)
#include_directories("/usr/include/python3.5m/")
#endif (UNIX)
#if (WIN32)
#include_directories("C:/Python27/include")
#include_directories("C:/Python35/include")
#set (PROJECTNAME_ARCHITECTURE "x64")
#endif (WIN32)
SET_SOURCE_FILES_PROPERTIES(python/od_system.i PROPERTIES CPLUSPLUS ON)
swig_add_library(odsystem LANGUAGE python SOURCES python/od_system.i)
SWIG_LINK_LIBRARIES(odsystem od_system)
SWIG_LINK_LIBRARIES(odsystem ${PYTHON_LIBRARIES})
#if (UNIX)
    #SWIG_LINK_LIBRARIES(odsystem /usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/libpython3.5m.so)
#endif (UNIX)
#if (WIN32)
#SWIG_LINK_LIBRARIES(odsystem "C:/Python27/libs/python27.lib")
#SWIG_LINK_LIBRARIES(odsystem "C:/Python35/libs/python35_d.lib")

