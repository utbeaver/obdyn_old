cmake_minimum_required (VERSION 3.5)
project (OD_SYSTEMD)

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
add_library(od_system SHARED ${OD_SRC})
install(TARGETS od_system DESTINATION /home/psong/obdyn/lib)
