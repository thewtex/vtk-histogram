cmake_minimum_required( VERSION 2.8 )

project( VTK_HISTOGRAM )

find_package( VTK REQUIRED )
include( ${VTK_USE_FILE} )

add_subdirectory( Code )
