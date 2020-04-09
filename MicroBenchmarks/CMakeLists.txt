file(COPY lit.local.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(libs)
# XRay benchmarks need to link with compiler-rt. We do not build compiler-rt as
# part of our nightly Checked C validations. So we need to skip XRay tests from
# running as part of benchmarking.
#add_subdirectory(XRay)
add_subdirectory(LCALS)
add_subdirectory(harris)
add_subdirectory(ImageProcessing)
add_subdirectory(LoopInterchange)
