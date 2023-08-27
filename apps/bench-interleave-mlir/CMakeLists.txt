add_executable(interlf-mlir-local local.cc visit_local.c)
target_link_libraries(interlf-mlir-local common)

add_executable(interlf-mlir-remote remote.cc)
target_link_libraries(interlf-mlir-remote common /users/Zijian/Disagg-mlir/tools/disagg/example/interleave/mlir/lower.o)

add_executable(interlf-mlir-data remote.cc)
target_link_libraries(interlf-mlir-data common /data/test/srcs/interleave/lower.o)

add_executable(interlf-mlir-remote-ref remote.cc visit_remote.c)
target_link_libraries(interlf-mlir-remote-ref common)
# add_library(rvisit OBJECT visit_remote.c)