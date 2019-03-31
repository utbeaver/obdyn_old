cmake_minimum_required (VERSION 3.5)
project (od_system)
set(CMAKE BUILD TYPE Debug)

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
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
#SET(CMAKE_SWIG_FLAGS "")

if (UNIX)
include_directories("/usr/include/python3.5m/")
endif (UNIX)
SET_SOURCE_FILES_PROPERTIES(python/od_system.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(odsystem python python/od_system.i)
SWIG_LINK_LIBRARIES(odsystem od_system)
if (UNIX)
SWIG_LINK_LIBRARIES(odsystem /usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/libpython3.5m.so)
endif (UNIX)
