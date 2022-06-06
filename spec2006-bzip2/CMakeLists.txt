project(spec2006-bzip2)

add_subdirectory(bzip2-1.0.3)
link_libraries(bzip2 common)
add_executable(csender compressor_send.cpp)
add_executable(crecver-kvs compressor_recv_kvs.cpp)
add_executable(crecver-norm compressor_recv_norm.cpp)
add_executable(clocal compressor_local.cpp)