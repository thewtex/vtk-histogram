add_executable(histogram histogram.cxx)
target_link_libraries(histogram
  vtkCommon
  vtkIO
  vtkRendering
  vtkCharts
  )
