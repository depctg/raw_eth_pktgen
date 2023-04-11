link_libraries(common)

add_executable(gemm-slow slow.c)
add_executable(gemm-fast fast.cc)
add_executable(gemm-ring ref-ring.c)
# add_executable(prefetch-opt-ring prefetch.cc)