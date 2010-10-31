cmake_minimum_required(VERSION 2.6)

project(HISTOGRAM)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(histogram histogram.cxx)
target_link_libraries(histogram vtkHybrid)
