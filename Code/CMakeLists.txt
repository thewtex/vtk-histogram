add_executable( histogram
  histogram.cxx
  histogramArgs.cxx
  )
target_link_libraries( histogram
  ${VTK_LIBRARIES}
  vtkmetaio
  )
install( TARGETS histogram
  RUNTIME DESTINATION bin )
