link_libraries(common)

add_executable(gemm ref.c)
add_executable(gemm-ring-cache ref-ring-cache.c)
add_executable(gemm-ring-direct ref-ring-direct.c)
add_executable(gemm-mlir-ring with-mlir.c)

target_link_libraries(
  gemm-mlir-ring
  /users/Zijian/Disagg-mlir/tools/disagg/example/gemm.o
)
# add_executable(prefetch-opt-ring prefetch.cc)