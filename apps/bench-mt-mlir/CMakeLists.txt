add_executable(mt-ref ref.cc)
add_executable(mt-remote remote.cc)

target_link_libraries(mt-ref
  common
  # tcmalloc
  /users/Zijian/Disagg-mlir/tools/disagg/example/matmul-mt/lower_local.o
)

target_link_libraries(mt-remote
  common
  # tcmalloc
  /users/Zijian/Disagg-mlir/tools/disagg/example/matmul-mt/mt/lower_remote.o
)
