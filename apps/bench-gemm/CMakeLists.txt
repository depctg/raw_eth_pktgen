link_libraries(common)

add_executable(gemm ref.c)
add_executable(gemm-ring ref-ring.c)
# add_executable(prefetch-opt-ring prefetch.cc)