link_libraries(common)

add_executable(mm-shared-mt mm-shared-mt.cc)
add_executable(mm-old-mt mm-old-mt.cc)
add_executable(mm-old-mt-test mm-old-mt-test.cc)
add_executable(mt-mlir mt-mlir.cc)

target_link_libraries(mt-mlir /users/Zijian/Disagg-mlir/tools/disagg/example/matmul-mt/mt/lower_remote.o)
# add_executable(mm-mt-gen ref-direct-mt.gen.cc)
# add_executable(prefetch-opt-ring prefetch.cc)
