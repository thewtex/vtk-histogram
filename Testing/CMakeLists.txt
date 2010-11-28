set( INPUT_DIR ${VTK_HISTOGRAM_SOURCE_DIR}/Testing/Data )

add_test( histogram_test
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/histogram
  ${INPUT_DIR}/foot_gradient_magnitude.mha
  )

add_test( histogram_test_options
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/histogram
  ${INPUT_DIR}/foot_gradient_magnitude.mha
  -p 3
  -q 30
  -n 50
  )
