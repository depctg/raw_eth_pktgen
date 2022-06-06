link_libraries(common)

add_executable(${app}-indmem-send indirect_mem_send.cpp)
add_executable(${app}-indmem-recv indirect_mem_recv.cpp)